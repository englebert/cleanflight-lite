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

#if defined(STM32F1) || defined(STM32F3) || defined(STM32F4)
#define MAX_SPI_PIN_SEL 2
#else
#define MAX_SPI_PIN_SEL 4
#endif

typedef struct spiPinDef_s {
    ioTag_t pin;
#ifdef STM32F7
    uint8_t af;
#endif
} spiPinDef_t;

typedef struct spiHardware_s {
    SPIDevice device;
    SPI_TypeDef *reg;
    spiPinDef_t sckPins[MAX_SPI_PIN_SEL];
    spiPinDef_t misoPins[MAX_SPI_PIN_SEL];
    spiPinDef_t mosiPins[MAX_SPI_PIN_SEL];
#ifndef STM32F7
    uint8_t af;
#endif
    rccPeriphTag_t rcc;
#if defined(USE_HAL_DRIVER)
    uint8_t dmaIrqHandler;
#endif
} spiHardware_t;

extern const spiHardware_t spiHardware[];

typedef struct SPIDevice_s {
    SPI_TypeDef *dev;
    ioTag_t sck;
    ioTag_t miso;
    ioTag_t mosi;
#ifdef STM32F7
    uint8_t sckAF;
    uint8_t misoAF;
    uint8_t mosiAF;
#else
    uint8_t af;
#endif
    rccPeriphTag_t rcc;
    volatile uint16_t errorCount;
    bool leadingEdge;
#if defined(USE_HAL_DRIVER)
    SPI_HandleTypeDef hspi;
    DMA_HandleTypeDef hdma;
    uint8_t dmaIrqHandler;
#endif
} spiDevice_t;

extern spiDevice_t spiDevice[SPIDEV_COUNT];

void spiInitDevice(SPIDevice device);
uint32_t spiTimeoutUserCallback(SPI_TypeDef *instance);
