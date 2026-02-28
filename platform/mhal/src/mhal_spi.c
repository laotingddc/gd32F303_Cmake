#include "mhal_spi.h"
#include "auto_init.h"

/* 板级 SPI 配置：直接在本文件展开，避免跨文件 extern 依赖 */
static const mhal_spi_cfg_t s_board_spi_cfg[] = {
#define SPI_ITEM(name, bus, mode, baud_div, cs_port, cs_pin) \
    [MHAL_SPI_ID_##name] = { (bus), (mode), (baud_div), (uint8_t)(cs_port), (uint16_t)(cs_pin) },
#include "mhal_spi_table.def"
#undef SPI_ITEM
};

static const size_t s_board_spi_cnt = MHAL_SPI_ID_MAX;

int mhal_spi_port_bus_init(mhal_spi_bus_t bus, mhal_spi_mode_t mode, mhal_spi_baud_div_t baud_div);
int mhal_spi_port_transfer_byte(mhal_spi_bus_t bus, uint8_t tx, uint8_t *rx);

typedef struct {
    bool configured;
    mhal_spi_mode_t mode;
    mhal_spi_baud_div_t baud_div;
} mhal_spi_bus_runtime_t;

static mhal_spi_bus_runtime_t s_spi_bus_runtime[3];

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

static bool _is_valid_id(uint32_t id)
{
    return (id < s_board_spi_cnt);
}

static bool _is_valid_cfg(const mhal_spi_cfg_t *cfg)
{
    if (cfg == NULL || cfg->cs_pin_mask == 0U) {
        return false;
    }

    if (!((cfg->bus == MHAL_SPI_BUS_1) || (cfg->bus == MHAL_SPI_BUS_2))) {
        return false;
    }

    if ((uint32_t)cfg->mode > (uint32_t)MHAL_SPI_MODE_3) {
        return false;
    }

    switch (cfg->baud_div) {
        case MHAL_SPI_BAUD_DIV_2:
        case MHAL_SPI_BAUD_DIV_4:
        case MHAL_SPI_BAUD_DIV_8:
        case MHAL_SPI_BAUD_DIV_16:
        case MHAL_SPI_BAUD_DIV_32:
        case MHAL_SPI_BAUD_DIV_64:
        case MHAL_SPI_BAUD_DIV_128:
        case MHAL_SPI_BAUD_DIV_256:
            return true;
        default:
            return false;
    }
}

static int _ensure_bus_runtime(const mhal_spi_cfg_t *cfg)
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
        if (mhal_spi_port_bus_init(cfg->bus, cfg->mode, cfg->baud_div) != 0) {
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
static int mhal_spi_init_from_cfg(void)
{
    uint8_t inited_bus_bitmap = 0U;

    if (s_board_spi_cnt == 0U) {
        return -1;
    }

    for (size_t i = 0; i < s_board_spi_cnt; i++) {
        const mhal_spi_cfg_t *cfg = &s_board_spi_cfg[i];
        if (!_is_valid_cfg(cfg)) {
            continue;
        }

        mhal_gpio_init(cfg->cs_port_idx, cfg->cs_pin_mask, MHAL_GPIO_MODE_OUT_PP);
        mhal_gpio_write(cfg->cs_port_idx, cfg->cs_pin_mask, MHAL_GPIO_HIGH);

        uint8_t bus_bit = (uint8_t)(1U << (uint8_t)cfg->bus);
        if ((inited_bus_bitmap & bus_bit) == 0U) {
            if (mhal_spi_port_bus_init(cfg->bus, cfg->mode, cfg->baud_div) != 0) {
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

INIT_BOARD_EXPORT_PRIO(mhal_spi_init_from_cfg, "70");

void mhal_spi_id_cs_select(uint32_t id)
{
    if (!_is_valid_id(id)) {
        return;
    }

    mhal_gpio_write(s_board_spi_cfg[id].cs_port_idx, s_board_spi_cfg[id].cs_pin_mask, MHAL_GPIO_LOW);
}

void mhal_spi_id_cs_release(uint32_t id)
{
    if (!_is_valid_id(id)) {
        return;
    }

    mhal_gpio_write(s_board_spi_cfg[id].cs_port_idx, s_board_spi_cfg[id].cs_pin_mask, MHAL_GPIO_HIGH);
}

int mhal_spi_id_transfer_no_cs(uint32_t id, const uint8_t *tx, uint8_t *rx, size_t len)
{
    if (!_is_valid_id(id) || (len == 0U)) {
        return -1;
    }

    const mhal_spi_cfg_t *cfg = &s_board_spi_cfg[id];
    if (_ensure_bus_runtime(cfg) != 0) {
        return -3;
    }

    for (size_t i = 0; i < len; i++) {
        uint8_t rx_byte = 0U;
        uint8_t tx_byte = (tx != NULL) ? tx[i] : 0xFFU;

        if (mhal_spi_port_transfer_byte(cfg->bus, tx_byte, &rx_byte) != 0) {
            return -2;
        }

        if (rx != NULL) {
            rx[i] = rx_byte;
        }
    }

    return 0;
}

int mhal_spi_id_transfer(uint32_t id, const uint8_t *tx, uint8_t *rx, size_t len)
{
    int ret;

    if (!_is_valid_id(id) || (len == 0U)) {
        return -1;
    }

    mhal_spi_id_cs_select(id);
    ret = mhal_spi_id_transfer_no_cs(id, tx, rx, len);
    mhal_spi_id_cs_release(id);

    return ret;
}

int mhal_spi_id_write(uint32_t id, const uint8_t *tx, size_t len)
{
    if ((tx == NULL) || (len == 0U)) {
        return -1;
    }

    return mhal_spi_id_transfer(id, tx, NULL, len);
}

int mhal_spi_id_read(uint32_t id, uint8_t *rx, size_t len, uint8_t filler)
{
    const mhal_spi_cfg_t *cfg;

    if (!_is_valid_id(id) || (rx == NULL) || (len == 0U)) {
        return -1;
    }

    cfg = &s_board_spi_cfg[id];
    if (_ensure_bus_runtime(cfg) != 0) {
        return -3;
    }

    mhal_spi_id_cs_select(id);
    for (size_t i = 0; i < len; i++) {
        if (mhal_spi_port_transfer_byte(cfg->bus, filler, &rx[i]) != 0) {
            mhal_spi_id_cs_release(id);
            return -2;
        }
    }
    mhal_spi_id_cs_release(id);

    return 0;
}
