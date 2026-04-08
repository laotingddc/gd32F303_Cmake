#include "hal_gpio.h"
#include "auto_init.h"

// 板级 GPIO 配置：直接在本文件展开，避免跨文件 extern 依赖
static const hal_gpio_cfg_t s_board_gpio_cfg[] = {
#define GPIO_ITEM(name, port, pin, mode, init_level) \
    [HAL_GPIO_ID_##name] = { (uint8_t)(port), (uint16_t)(pin), (mode), (init_level) },
#include "gpio_table.def"
#undef GPIO_ITEM
};

static const size_t s_board_gpio_cnt = HAL_GPIO_ID_MAX;

/**
 * @brief 使用板级配置表初始化 GPIO
 */
static int hal_gpio_init_from_cfg(void) {
    if (s_board_gpio_cnt == 0) {
        return -1;
    }

    for (size_t i = 0; i < s_board_gpio_cnt; i++) {
        hal_gpio_init(s_board_gpio_cfg[i].port_idx, s_board_gpio_cfg[i].pin_mask, s_board_gpio_cfg[i].mode);
        hal_gpio_write(s_board_gpio_cfg[i].port_idx, s_board_gpio_cfg[i].pin_mask, s_board_gpio_cfg[i].init_level);
    }

    return 0;
}

INIT_BOARD_EXPORT_PRIO(hal_gpio_init_from_cfg, "60");

/**
 * @brief 基于 ID 的写操作
 */
void hal_gpio_id_write(uint32_t id, hal_gpio_status_t status) {
    if (id < s_board_gpio_cnt) {
        hal_gpio_write(s_board_gpio_cfg[id].port_idx, s_board_gpio_cfg[id].pin_mask, status);
    }
}

/**
 * @brief 基于 ID 的翻转操作
 */
void hal_gpio_id_toggle(uint32_t id) {
    if (id < s_board_gpio_cnt) {
        hal_gpio_toggle(s_board_gpio_cfg[id].port_idx, s_board_gpio_cfg[id].pin_mask);
    }
}

/**
 * @brief 基于 ID 的读操作
 */
hal_gpio_status_t hal_gpio_id_read(uint32_t id) {
    if (id < s_board_gpio_cnt) {
        return hal_gpio_read(s_board_gpio_cfg[id].port_idx, s_board_gpio_cfg[id].pin_mask);
    }
    return HAL_GPIO_LOW;
}
