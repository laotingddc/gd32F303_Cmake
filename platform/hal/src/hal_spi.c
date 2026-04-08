#include "hal_spi.h"
#include "auto_init.h"

/* 板级 SPI 配置：直接在本文件展开，避免跨文件 extern 依赖 */
static const hal_spi_cfg_t s_board_spi_cfg[] = {
#define SPI_ITEM(name, bus, mode, baud_div, cs_port, cs_pin) \
    [HAL_SPI_ID_##name] = { (bus), (mode), (baud_div), (uint8_t)(cs_port), (uint16_t)(cs_pin) },
#include "spi_table.def"
#undef SPI_ITEM
};

static const size_t s_board_spi_cnt = HAL_SPI_ID_MAX;

int hal_spi_port_bus_init(hal_spi_bus_t bus, hal_spi_mode_t mode, hal_spi_baud_div_t baud_div);
int hal_spi_port_transfer_byte(hal_spi_bus_t bus, uint8_t tx, uint8_t *rx);

typedef struct {
    bool configured;
    hal_spi_mode_t mode;
    hal_spi_baud_div_t baud_div;
} hal_spi_bus_runtime_t;

static hal_spi_bus_runtime_t s_spi_bus_runtime[3];

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

static bool _is_valid_id(uint32_t id)
{
    return (id < s_board_spi_cnt);
}

static bool _is_valid_cfg(const hal_spi_cfg_t *cfg)
{
    if (cfg == NULL || cfg->cs_pin_mask == 0U) {
        return false;
    }

    if (!((cfg->bus == HAL_SPI_BUS_1) || (cfg->bus == HAL_SPI_BUS_2))) {
        return false;
    }

    if ((uint32_t)cfg->mode > (uint32_t)HAL_SPI_MODE_3) {
        return false;
    }

    switch (cfg->baud_div) {
        case HAL_SPI_BAUD_DIV_2:
        case HAL_SPI_BAUD_DIV_4:
        case HAL_SPI_BAUD_DIV_8:
        case HAL_SPI_BAUD_DIV_16:
        case HAL_SPI_BAUD_DIV_32:
        case HAL_SPI_BAUD_DIV_64:
        case HAL_SPI_BAUD_DIV_128:
        case HAL_SPI_BAUD_DIV_256:
            return true;
        default:
            return false;
    }
}

static int _ensure_bus_runtime(const hal_spi_cfg_t *cfg)
{
    uint8_t bus_idx;

    if (cfg == NULL) {
        return -1;
    }

    bus_idx = (uint8_t)cfg->bus;
    if (bus_idx >= ARRAY_SIZE(s_spi_bus_runtime)) {
        return -1;
    }

    if (!s_spi_bus_runtime[bus_idx].configured ||
        (s_spi_bus_runtime[bus_idx].mode != cfg->mode) ||
        (s_spi_bus_runtime[bus_idx].baud_div != cfg->baud_div)) {
        if (hal_spi_port_bus_init(cfg->bus, cfg->mode, cfg->baud_div) != 0) {
            return -2;
        }

        s_spi_bus_runtime[bus_idx].configured = true;
        s_spi_bus_runtime[bus_idx].mode = cfg->mode;
        s_spi_bus_runtime[bus_idx].baud_div = cfg->baud_div;
    }

    return 0;
}

/**
 * @brief 使用板级配置表初始化 SPI
 */
static int hal_spi_init_from_cfg(void)
{
    uint8_t inited_bus_bitmap = 0U;

    if (s_board_spi_cnt == 0U) {
        return -1;
    }

    for (size_t i = 0; i < s_board_spi_cnt; i++) {
        const hal_spi_cfg_t *cfg = &s_board_spi_cfg[i];
        if (!_is_valid_cfg(cfg)) {
            continue;
        }

        hal_gpio_init(cfg->cs_port_idx, cfg->cs_pin_mask, HAL_GPIO_MODE_OUT_PP);
        hal_gpio_write(cfg->cs_port_idx, cfg->cs_pin_mask, HAL_GPIO_HIGH);

        uint8_t bus_bit = (uint8_t)(1U << (uint8_t)cfg->bus);
        if ((inited_bus_bitmap & bus_bit) == 0U) {
            if (hal_spi_port_bus_init(cfg->bus, cfg->mode, cfg->baud_div) != 0) {
                return -2;
            }

            s_spi_bus_runtime[(uint8_t)cfg->bus].configured = true;
            s_spi_bus_runtime[(uint8_t)cfg->bus].mode = cfg->mode;
            s_spi_bus_runtime[(uint8_t)cfg->bus].baud_div = cfg->baud_div;

            inited_bus_bitmap = (uint8_t)(inited_bus_bitmap | bus_bit);
        }
    }

    return 0;
}

INIT_BOARD_EXPORT_PRIO(hal_spi_init_from_cfg, "70");

void hal_spi_id_cs_select(uint32_t id)
{
    if (!_is_valid_id(id)) {
        return;
    }

    hal_gpio_write(s_board_spi_cfg[id].cs_port_idx, s_board_spi_cfg[id].cs_pin_mask, HAL_GPIO_LOW);
}

void hal_spi_id_cs_release(uint32_t id)
{
    if (!_is_valid_id(id)) {
        return;
    }

    hal_gpio_write(s_board_spi_cfg[id].cs_port_idx, s_board_spi_cfg[id].cs_pin_mask, HAL_GPIO_HIGH);
}

int hal_spi_id_transfer_no_cs(uint32_t id, const uint8_t *tx, uint8_t *rx, size_t len)
{
    if (!_is_valid_id(id) || (len == 0U)) {
        return -1;
    }

    const hal_spi_cfg_t *cfg = &s_board_spi_cfg[id];
    if (_ensure_bus_runtime(cfg) != 0) {
        return -3;
    }

    for (size_t i = 0; i < len; i++) {
        uint8_t rx_byte = 0U;
        uint8_t tx_byte = (tx != NULL) ? tx[i] : 0xFFU;

        if (hal_spi_port_transfer_byte(cfg->bus, tx_byte, &rx_byte) != 0) {
            return -2;
        }

        if (rx != NULL) {
            rx[i] = rx_byte;
        }
    }

    return 0;
}

int hal_spi_id_transfer(uint32_t id, const uint8_t *tx, uint8_t *rx, size_t len)
{
    int ret;

    if (!_is_valid_id(id) || (len == 0U)) {
        return -1;
    }

    hal_spi_id_cs_select(id);
    ret = hal_spi_id_transfer_no_cs(id, tx, rx, len);
    hal_spi_id_cs_release(id);

    return ret;
}

int hal_spi_id_write(uint32_t id, const uint8_t *tx, size_t len)
{
    if ((tx == NULL) || (len == 0U)) {
        return -1;
    }

    return hal_spi_id_transfer(id, tx, NULL, len);
}

int hal_spi_id_read(uint32_t id, uint8_t *rx, size_t len, uint8_t filler)
{
    const hal_spi_cfg_t *cfg;

    if (!_is_valid_id(id) || (rx == NULL) || (len == 0U)) {
        return -1;
    }

    cfg = &s_board_spi_cfg[id];
    if (_ensure_bus_runtime(cfg) != 0) {
        return -3;
    }

    hal_spi_id_cs_select(id);
    for (size_t i = 0; i < len; i++) {
        if (hal_spi_port_transfer_byte(cfg->bus, filler, &rx[i]) != 0) {
            hal_spi_id_cs_release(id);
            return -2;
        }
    }
    hal_spi_id_cs_release(id);

    return 0;
}
