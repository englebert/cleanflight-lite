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

#pragma once

#include "drivers/io_types.h"

typedef enum softSPIDevice {
    SOFT_SPIDEV_1   = 0
} softSPIDevice_e;

typedef struct softSPIDevice_s {
    ioTag_t sckTag;
    ioTag_t mosiTag;
    ioTag_t misoTag;
    ioTag_t nssTag;
} softSPIDevice_t;

// (See rxNRF24Init for an example.)
typedef struct SoftSPIDevice {
    GPIO_TypeDef* csn_port;
    GPIO_TypeDef* sck_port;
    GPIO_TypeDef* miso_port;
    GPIO_TypeDef* mosi_port;

    uint16_t csn_pin;
    uint16_t sck_pin;
    uint16_t miso_pin;
    uint16_t mosi_pin;
} SoftSPIDevice;


// Convenience macros to set pins high or low,
// where 'dev' is an instance of SoftSPIDevice
#define SOFTSPI_CSN_L(dev)  GPIO_ResetBits(dev.csn_port,dev.csn_pin)
#define SOFTSPI_CSN_H(dev)  GPIO_SetBits(dev.csn_port,dev.csn_pin)

#define SOFTSPI_SCK_L(dev)  GPIO_ResetBits(dev.sck_port,dev.sck_pin)
#define SOFTSPI_SCK_H(dev)  GPIO_SetBits(dev.sck_port,dev.sck_pin)

#define SOFTSPI_MISO_L(dev) GPIO_ResetBits(dev.miso_port,dev.miso_pin)
#define SOFTSPI_MISO_H(dev) GPIO_SetBits(dev.miso_port,dev.miso_pin)

#define SOFTSPI_MOSI_L(dev) GPIO_ResetBits(dev.mosi_port,dev.mosi_pin)
#define SOFTSPI_MOSI_H(dev) GPIO_SetBits(dev.mosi_port,dev.mosi_pin)

#define SOFTSPI_MISO_R(dev) (GPIO_ReadInputDataBit(dev.miso_port,dev.miso_pin))



void softSpiInit(const softSPIDevice_t *dev);
uint8_t softSpiTransferByte(const softSPIDevice_t *dev, uint8_t data);

// Porting from Old NRF code
void softSpiInit_NRF(SoftSPIDevice* dev);
uint8_t softSpiTransferByte_NRF(SoftSPIDevice *dev, uint8_t in);

