#include "mhal_gpio.h"
#include "gd32f30x.h"
#include <stdint.h>

// 声明由外部生成的配置
extern const mhal_gpio_cfg_t g_board_gpio_cfg[];
extern const size_t g_board_gpio_cnt;

// 内部映射辅助：端口索引转 GD32 端口地址
static uint32_t _get_port_base(uint8_t port_idx) {
    switch (port_idx) {
        case 0: return GPIOA;
        case 1: return GPIOB;
        case 2: return GPIOC;
        case 3: return GPIOD;
        case 4: return GPIOE;
        case 5: return GPIOF;
        case 6: return GPIOG;
        default: return GPIOA;
    }
}

// 内部映射辅助：RCU 转换
static rcu_periph_enum _get_rcu_port(uint8_t port_idx) {
    switch (port_idx) {
        case 0: return RCU_GPIOA;
        case 1: return RCU_GPIOB;
        case 2: return RCU_GPIOC;
        case 3: return RCU_GPIOD;
        case 4: return RCU_GPIOE;
        case 5: return RCU_GPIOF;
        case 6: return RCU_GPIOG;
        default: return RCU_GPIOA;
    }
}

void mhal_gpio_init(uint8_t port_idx, uint16_t pin_mask, mhal_gpio_mode_t mode) {
    uint32_t port = _get_port_base(port_idx);
    rcu_periph_clock_enable(_get_rcu_port(port_idx));
    
    uint32_t gd_mode;
    switch (mode) {
        case MHAL_GPIO_MODE_OUT_PP:      gd_mode = GPIO_MODE_OUT_PP; break;
        case MHAL_GPIO_MODE_IN_FLOATING: gd_mode = GPIO_MODE_IN_FLOATING; break;
        case MHAL_GPIO_MODE_IN_PU:       gd_mode = GPIO_MODE_IPU; break;
        case MHAL_GPIO_MODE_IN_PD:       gd_mode = GPIO_MODE_IPD; break;
        default: gd_mode = GPIO_MODE_OUT_PP; break;
    }
    
    gpio_init(port, gd_mode, GPIO_OSPEED_50MHZ, pin_mask);
}

void mhal_gpio_write(uint8_t port_idx, uint16_t pin_mask, mhal_gpio_status_t status) {
    gpio_bit_write(_get_port_base(port_idx), pin_mask, (bit_status)status);
}

mhal_gpio_status_t mhal_gpio_read(uint8_t port_idx, uint16_t pin_mask) {
    return (gpio_input_bit_get(_get_port_base(port_idx), pin_mask) == SET) ? MHAL_GPIO_HIGH : MHAL_GPIO_LOW;
}

void mhal_gpio_toggle(uint8_t port_idx, uint16_t pin_mask) {
    mhal_gpio_status_t status = mhal_gpio_read(port_idx, pin_mask);
    mhal_gpio_write(port_idx, pin_mask, (status == MHAL_GPIO_HIGH) ? MHAL_GPIO_LOW : MHAL_GPIO_HIGH);
}

void mhal_gpio_load_cfg(const mhal_gpio_cfg_t *cfg_array, size_t count) {
    for (size_t i = 0; i < count; i++) {
        mhal_gpio_init(cfg_array[i].port_idx, cfg_array[i].pin_mask, cfg_array[i].mode);
        mhal_gpio_write(cfg_array[i].port_idx, cfg_array[i].pin_mask, cfg_array[i].init_level);
    }
}

// 基于 ID 的操作实现
void mhal_gpio_id_write(uint32_t id, mhal_gpio_status_t status) {
    if (id < g_board_gpio_cnt) {
        mhal_gpio_write(g_board_gpio_cfg[id].port_idx, g_board_gpio_cfg[id].pin_mask, status);
    }
}

void mhal_gpio_id_toggle(uint32_t id) {
    if (id < g_board_gpio_cnt) {
        mhal_gpio_toggle(g_board_gpio_cfg[id].port_idx, g_board_gpio_cfg[id].pin_mask);
    }
}

mhal_gpio_status_t mhal_gpio_id_read(uint32_t id) {
    if (id < g_board_gpio_cnt) {
        return mhal_gpio_read(g_board_gpio_cfg[id].port_idx, g_board_gpio_cfg[id].pin_mask);
    }
    return MHAL_GPIO_LOW;
}
