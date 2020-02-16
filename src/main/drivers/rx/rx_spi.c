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

// This file is copied with modifications from project Deviation,
// see http://deviationtx.com

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <platform.h>

#ifdef USE_RX_SPI

#include "build/build_config.h"

#include "drivers/bus_spi.h"
#include "drivers/bus_spi_soft.h"
#include "drivers/io.h"
#include "drivers/io_impl.h"
#include "drivers/rcc.h"
#include "drivers/system.h"

#include "rx_spi.h"

#define DISABLE_RX()    {IOHi(DEFIO_IO(RX_NSS_PIN));}
#define ENABLE_RX()     {IOLo(DEFIO_IO(RX_NSS_PIN));}

#ifdef USE_RX_SOFTSPI
static const softSPIDevice_t softSPIDevice = {
    .sckTag = IO_TAG(RX_SCK_PIN),
    .mosiTag = IO_TAG(RX_MOSI_PIN),
    .misoTag = IO_TAG(RX_MISO_PIN),
    // Note: Nordic Semiconductor uses 'CSN', STM uses 'NSS'
    .nssTag = IO_TAG(RX_NSS_PIN),
};
static bool useSoftSPI = false;
#endif // USE_RX_SOFTSPI

void rxSpiDeviceInit(rx_spi_type_e spiType)
{
    static bool hardwareInitialised = false;

    if (hardwareInitialised) {
        return;
    }

#ifdef USE_RX_SOFTSPI
    if (spiType == RX_SPI_SOFTSPI) {
        useSoftSPI = true;
        softSpiInit(&softSPIDevice);
    }
    const SPIDevice rxSPIDevice = SOFT_SPIDEV_1;
#else
    UNUSED(spiType);
    const SPIDevice rxSPIDevice = spiDeviceByInstance(RX_SPI_INSTANCE);
    const IO_t rxCsPin = DEFIO_IO(RX_NSS_PIN);
    IOInit(rxCsPin, OWNER_RX_SPI_CS, rxSPIDevice + 1);
    IOConfigGPIO(rxCsPin, SPI_IO_CS_CFG);
#endif // USE_RX_SOFTSPI

    DISABLE_RX();

#ifdef RX_SPI_INSTANCE
    spiSetDivisor(RX_SPI_INSTANCE, SPI_CLOCK_STANDARD);
#endif
    hardwareInitialised = true;
}

uint8_t rxSpiTransferByte(uint8_t data)
{
#ifdef USE_RX_SOFTSPI
    if (useSoftSPI) {
        return softSpiTransferByte(&softSPIDevice, data);
    } else
#endif
    {
#ifdef RX_SPI_INSTANCE
        return spiTransferByte(RX_SPI_INSTANCE, data);
#else
        return 0;
#endif
    }
}

uint8_t rxSpiWriteByte(uint8_t data)
{
    ENABLE_RX();
    const uint8_t ret = rxSpiTransferByte(data);
    DISABLE_RX();
    return ret;
}

uint8_t rxSpiWriteCommand(uint8_t command, uint8_t data)
{
    ENABLE_RX();
    const uint8_t ret = rxSpiTransferByte(command);
    rxSpiTransferByte(data);
    DISABLE_RX();
    return ret;
}

uint8_t rxSpiWriteCommandMulti(uint8_t command, const uint8_t *data, uint8_t length)
{
    ENABLE_RX();
    const uint8_t ret = rxSpiTransferByte(command);
    for (uint8_t i = 0; i < length; i++) {
        rxSpiTransferByte(data[i]);
    }
    DISABLE_RX();
    return ret;
}

uint8_t rxSpiReadCommand(uint8_t command, uint8_t data)
{
    ENABLE_RX();
    rxSpiTransferByte(command);
    const uint8_t ret = rxSpiTransferByte(data);
    DISABLE_RX();
    return ret;
}

uint8_t rxSpiReadCommandMulti(uint8_t command, uint8_t commandData, uint8_t *retData, uint8_t length)
{
    ENABLE_RX();
    const uint8_t ret = rxSpiTransferByte(command);
    for (uint8_t i = 0; i < length; i++) {
        retData[i] = rxSpiTransferByte(commandData);
    }
    DISABLE_RX();
    return ret;
}

void softSpiInit(SoftSPIDevice *dev)
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


uint8_t softSpiTransferByte(SoftSPIDevice *dev, uint8_t byte)
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
