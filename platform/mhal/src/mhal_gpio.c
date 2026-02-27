#include "mhal_gpio.h"
#include "auto_init.h"

// 板级 GPIO 配置：直接在本文件展开，避免跨文件 extern 依赖
static const mhal_gpio_cfg_t s_board_gpio_cfg[] = {
#define GPIO_ITEM(name, port, pin, mode, init_level) \
    [MHAL_GPIO_ID_##name] = { (uint8_t)(port), (uint16_t)(pin), (mode), (init_level) },
#include "mhal_gpio_table.def"
#undef GPIO_ITEM
};

static const size_t s_board_gpio_cnt = MHAL_GPIO_ID_MAX;

/**
 * @brief 使用板级配置表初始化 GPIO
 */
static int mhal_gpio_init_from_cfg(void) {
    if (s_board_gpio_cnt == 0) {
        return -1;
    }

    for (size_t i = 0; i < s_board_gpio_cnt; i++) {
        mhal_gpio_init(s_board_gpio_cfg[i].port_idx, s_board_gpio_cfg[i].pin_mask, s_board_gpio_cfg[i].mode);
        mhal_gpio_write(s_board_gpio_cfg[i].port_idx, s_board_gpio_cfg[i].pin_mask, s_board_gpio_cfg[i].init_level);
    }

    return 0;
}

INIT_BOARD_EXPORT_PRIO(mhal_gpio_init_from_cfg, "60");

/**
 * @brief 基于 ID 的写操作
 */
void mhal_gpio_id_write(uint32_t id, mhal_gpio_status_t status) {
    if (id < s_board_gpio_cnt) {
        mhal_gpio_write(s_board_gpio_cfg[id].port_idx, s_board_gpio_cfg[id].pin_mask, status);
    }
}

/**
 * @brief 基于 ID 的翻转操作
 */
void mhal_gpio_id_toggle(uint32_t id) {
    if (id < s_board_gpio_cnt) {
        mhal_gpio_toggle(s_board_gpio_cfg[id].port_idx, s_board_gpio_cfg[id].pin_mask);
    }
}

/**
 * @brief 基于 ID 的读操作
 */
mhal_gpio_status_t mhal_gpio_id_read(uint32_t id) {
    if (id < s_board_gpio_cnt) {
        return mhal_gpio_read(s_board_gpio_cfg[id].port_idx, s_board_gpio_cfg[id].pin_mask);
    }
    return MHAL_GPIO_LOW;
}
