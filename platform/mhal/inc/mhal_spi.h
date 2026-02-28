#ifndef MHAL_SPI_H
#define MHAL_SPI_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "mhal_gpio.h"

typedef enum {
    MHAL_SPI_BUS_1 = 1,
    MHAL_SPI_BUS_2 = 2,
} mhal_spi_bus_t;

typedef enum {
    MHAL_SPI_MODE_0 = 0,
    MHAL_SPI_MODE_1,
    MHAL_SPI_MODE_2,
    MHAL_SPI_MODE_3,
} mhal_spi_mode_t;

typedef enum {
    MHAL_SPI_BAUD_DIV_2 = 2,
    MHAL_SPI_BAUD_DIV_4 = 4,
    MHAL_SPI_BAUD_DIV_8 = 8,
    MHAL_SPI_BAUD_DIV_16 = 16,
    MHAL_SPI_BAUD_DIV_32 = 32,
    MHAL_SPI_BAUD_DIV_64 = 64,
    MHAL_SPI_BAUD_DIV_128 = 128,
    MHAL_SPI_BAUD_DIV_256 = 256,
} mhal_spi_baud_div_t;

typedef struct {
    mhal_spi_bus_t bus;
    mhal_spi_mode_t mode;
    mhal_spi_baud_div_t baud_div;
    uint8_t cs_port_idx;
    uint16_t cs_pin_mask;
} mhal_spi_cfg_t;

/* 板级 SPI ID（由 mhal_spi_table.def 展开） */
typedef enum {
#define SPI_ITEM(name, bus, mode, baud_div, cs_port, cs_pin) MHAL_SPI_ID_##name,
#include "mhal_spi_table.def"
#undef SPI_ITEM
    MHAL_SPI_ID_MAX
} mhal_spi_id_t;

void mhal_spi_id_cs_select(uint32_t id);
void mhal_spi_id_cs_release(uint32_t id);

int mhal_spi_id_transfer_no_cs(uint32_t id, const uint8_t *tx, uint8_t *rx, size_t len);
int mhal_spi_id_transfer(uint32_t id, const uint8_t *tx, uint8_t *rx, size_t len);
int mhal_spi_id_write(uint32_t id, const uint8_t *tx, size_t len);
int mhal_spi_id_read(uint32_t id, uint8_t *rx, size_t len, uint8_t filler);

#endif // MHAL_SPI_H
