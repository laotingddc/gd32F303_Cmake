#ifndef MHAL_GPIO_H
#define MHAL_GPIO_H

#include <stdint.h>
#include <stdbool.h>

// GPIO 电平状态
typedef enum {
    MHAL_GPIO_LOW = 0,
    MHAL_GPIO_HIGH = 1
} mhal_gpio_status_t;

// GPIO 模式定义 (简化版)
typedef enum {
    MHAL_GPIO_MODE_OUT_PP,  // 推挽输出
    MHAL_GPIO_MODE_IN_FLOATING, // 浮空输入
    MHAL_GPIO_MODE_IN_PU,   // 上拉输入
    MHAL_GPIO_MODE_IN_PD,   // 下拉输入
} mhal_gpio_mode_t;

/**
 * @brief 初始化 GPIO
 * @param port_idx 端口索引 (如 0 代表 A, 1 代表 B)
 * @param pin_mask 管脚掩码 (如 1<<8 代表 Pin8)
 * @param mode 模式
 */
void mhal_gpio_init(uint8_t port_idx, uint16_t pin_mask, mhal_gpio_mode_t mode);

/**
 * @brief 写 GPIO 电平
 */
void mhal_gpio_write(uint8_t port_idx, uint16_t pin_mask, mhal_gpio_status_t status);

/**
 * @brief 读 GPIO 电平
 */
mhal_gpio_status_t mhal_gpio_read(uint8_t port_idx, uint16_t pin_mask);

/**
 * @brief 翻转 GPIO 电平
 */
void mhal_gpio_toggle(uint8_t port_idx, uint16_t pin_mask);

#endif // MHAL_GPIO_H
