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

#include "platform.h"
#include "drivers/bus.h"
#include "drivers/bus_i2c.h"
#include "drivers/bus_spi.h"
#include "io/serial.h"
#include "pg/bus_i2c.h"
#include "pg/bus_spi.h"


extern void spiPreInit(void); // XXX In fc/fc_init.c

void targetBusInit(void)
{
#if defined(USE_SPI) && defined(USE_SPI_DEVICE_1)
    spiPinConfigure(spiPinConfig());
    spiPreInit();
    spiInit(SPIDEV_1);
#endif

    if (!doesConfigurationUsePort(SERIAL_PORT_USART3)) {
        serialRemovePort(SERIAL_PORT_USART3);
        i2cHardwareConfigure(i2cConfig());
        i2cInit(I2C_DEVICE);
    }
}
