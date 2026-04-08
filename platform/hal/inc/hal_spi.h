#ifndef HAL_SPI_H
#define HAL_SPI_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "hal_gpio.h"

typedef enum {
    HAL_SPI_BUS_1 = 1,
    HAL_SPI_BUS_2 = 2,
} hal_spi_bus_t;

typedef enum {
    HAL_SPI_MODE_0 = 0,
    HAL_SPI_MODE_1,
    HAL_SPI_MODE_2,
    HAL_SPI_MODE_3,
} hal_spi_mode_t;

typedef enum {
    HAL_SPI_BAUD_DIV_2 = 2,
    HAL_SPI_BAUD_DIV_4 = 4,
    HAL_SPI_BAUD_DIV_8 = 8,
    HAL_SPI_BAUD_DIV_16 = 16,
    HAL_SPI_BAUD_DIV_32 = 32,
    HAL_SPI_BAUD_DIV_64 = 64,
    HAL_SPI_BAUD_DIV_128 = 128,
    HAL_SPI_BAUD_DIV_256 = 256,
} hal_spi_baud_div_t;

typedef struct {
    hal_spi_bus_t bus;
    hal_spi_mode_t mode;
    hal_spi_baud_div_t baud_div;
    uint8_t cs_port_idx;
    uint16_t cs_pin_mask;
} hal_spi_cfg_t;

/* 板级 SPI ID（由 spi_table.def 展开） */
typedef enum {
#define SPI_ITEM(name, bus, mode, baud_div, cs_port, cs_pin) HAL_SPI_ID_##name,
#include "spi_table.def"
#undef SPI_ITEM
    HAL_SPI_ID_MAX
} hal_spi_id_t;

void hal_spi_id_cs_select(uint32_t id);
void hal_spi_id_cs_release(uint32_t id);

int hal_spi_id_transfer_no_cs(uint32_t id, const uint8_t *tx, uint8_t *rx, size_t len);
int hal_spi_id_transfer(uint32_t id, const uint8_t *tx, uint8_t *rx, size_t len);
int hal_spi_id_write(uint32_t id, const uint8_t *tx, size_t len);
int hal_spi_id_read(uint32_t id, uint8_t *rx, size_t len, uint8_t filler);

#endif // HAL_SPI_H
