F446_TARGETS  += $(TARGET)
FEATURES    = VCP

TARGET_SRC = \
            drivers/accgyro/accgyro_mpu6500.c \
            drivers/accgyro/accgyro_spi_mpu6500.c \
            drivers/accgyro/accgyro_spi_mpu9250.c \
            drivers/accgyro/accgyro_fake.c \
            rx/nrf24_inav.c \
            rx/nrf24_h8_3d.c \
            rx/nrf24_syma.c \
            rx/nRF24L01.c
