/*
 * This file is part of Cleanflight.
 *
 * Cleanflight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cleanflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cleanflight.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdbool.h>
#include <stdint.h>

#include <platform.h>

#ifdef USE_SOFTSPI

#include "build/build_config.h"


#include "drivers/io.h"
#include "io_impl.h"
#include "drivers/bus_spi.h"
#include "bus_spi_soft.h"


void softSpiInit(const softSPIDevice_t *dev)
{
    // SCK as output
    IOInit(IOGetByTag(dev->sckTag),  OWNER_SPI_SCK,  RESOURCE_INDEX(SOFT_SPIDEV_1) + 10);
#if defined(STM32F10X)
    IOConfigGPIO(IOGetByTag(dev->sckTag), IO_CONFIG(GPIO_Mode_Out_PP, GPIO_Speed_50MHz));
#elif defined(STM32F3) || defined(STM32F4)
    IOConfigGPIOAF(IOGetByTag(dev->sckTag), SPI_IO_AF_CFG, 0);
#endif

    // MOSI as output
    IOInit(IOGetByTag(dev->mosiTag),  OWNER_SPI_MOSI,  RESOURCE_INDEX(SOFT_SPIDEV_1) + 10);
#if defined(STM32F10X)
    IOConfigGPIO(IOGetByTag(dev->mosiTag), IO_CONFIG(GPIO_Mode_Out_PP, GPIO_Speed_50MHz));
#elif defined(STM32F3) || defined(STM32F4)
    IOConfigGPIOAF(IOGetByTag(dev->mosiTag), SPI_IO_AF_CFG, 0);
#endif

    // MISO as input
    IOInit(IOGetByTag(dev->misoTag),  OWNER_SPI_MISO,  RESOURCE_INDEX(SOFT_SPIDEV_1) + 10);
#if defined(STM32F10X)
    IOConfigGPIO(IOGetByTag(dev->misoTag), IO_CONFIG(GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz));
#elif defined(STM32F3) || defined(STM32F4)
    IOConfigGPIOAF(IOGetByTag(dev->misoTag), SPI_IO_AF_CFG, 0);
#endif

    // NSS as output
    // if (dev->nssTag != IOTAG_NONE) {
        IOInit(IOGetByTag(dev->nssTag),  OWNER_SPI_CS,  RESOURCE_INDEX(SOFT_SPIDEV_1) + 10);
#if defined(STM32F10X)
        IOConfigGPIO(IOGetByTag(dev->nssTag), IO_CONFIG(GPIO_Mode_Out_PP, GPIO_Speed_50MHz));
#elif defined(STM32F3) || defined(STM32F4)
        IOConfigGPIOAF(IOGetByTag(dev->nssTag), SPI_IO_AF_CFG, 0);
#endif
    //}
}

uint8_t softSpiTransferByte(const softSPIDevice_t *dev, uint8_t byte)
{
    for (int ii = 0; ii < 8; ++ii) {
        if (byte & 0x80) {
            IOHi(IOGetByTag(dev->mosiTag));
        } else {
            IOLo(IOGetByTag(dev->mosiTag));
        }
        IOHi(IOGetByTag(dev->sckTag));
        byte <<= 1;
        if (IORead(IOGetByTag(dev->misoTag)) == 1) {
            byte |= 1;
        }
        IOLo(IOGetByTag(dev->sckTag));
    }
    return byte;
}

void softSpiInit_NRF(SoftSPIDevice *dev)
{
    GPIO_InitTypeDef PORT;
    PORT.GPIO_Speed = GPIO_Speed_50MHz;

    PORT.GPIO_Pin = dev->csn_pin;
    PORT.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(dev->csn_port, &PORT);

    PORT.GPIO_Pin = dev->sck_pin;
    PORT.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(dev->sck_port, &PORT);

    PORT.GPIO_Pin = dev->miso_pin;
    PORT.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(dev->miso_port, &PORT);

    PORT.GPIO_Pin = dev->mosi_pin;
    PORT.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(dev->mosi_port, &PORT);
}


uint8_t softSpiTransferByte_NRF(SoftSPIDevice *dev, uint8_t byte)
{
    for(int i = 0; i < 8; i++) {

        if(byte & 0x80)
        {   
            GPIO_SetBits(dev->mosi_port, dev->mosi_pin);
        }   
        else
        {   
            GPIO_ResetBits(dev->mosi_port, dev->mosi_pin);
        }   

        GPIO_SetBits(dev->sck_port, dev->sck_pin);    
        byte <<= 1;

        if(GPIO_ReadInputDataBit(dev->miso_port, dev->miso_pin) == 1)
        {   
            byte |= 1;
        }   

        GPIO_ResetBits(dev->sck_port, dev->sck_pin);
    }   

    return byte;
}
#endif
