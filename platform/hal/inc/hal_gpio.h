#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// GPIO 电平状态
typedef enum {
    HAL_GPIO_LOW = 0,
    HAL_GPIO_HIGH = 1
} hal_gpio_status_t;

// GPIO 模式定义
typedef enum {
    HAL_GPIO_MODE_OUT_PP,
    HAL_GPIO_MODE_IN_FLOATING,
    HAL_GPIO_MODE_IN_PU,
    HAL_GPIO_MODE_IN_PD,
} hal_gpio_mode_t;

// GPIO 单项配置结构体
typedef struct {
    uint8_t port_idx;
    uint16_t pin_mask;
    hal_gpio_mode_t mode;
    hal_gpio_status_t init_level;
} hal_gpio_cfg_t;

/* 板级 GPIO 端口索引（与 port_gpio.c 的映射保持一致） */
enum {
    GPIO_PORT_A = 0,
    GPIO_PORT_B,
    GPIO_PORT_C,
    GPIO_PORT_D,
    GPIO_PORT_E,
    GPIO_PORT_F,
    GPIO_PORT_G,
};

#define GPIO_PIN(n) ((uint16_t)(1U << (n)))

/* 板级 GPIO ID（由 gpio_table.def 展开） */
typedef enum {
#define GPIO_ITEM(name, port, pin, mode, init_level) HAL_GPIO_ID_##name,
#include "gpio_table.def"
#undef GPIO_ITEM
    HAL_GPIO_ID_MAX
} hal_gpio_id_t;

// 基础物理操作接口
void hal_gpio_init(uint8_t port_idx, uint16_t pin_mask, hal_gpio_mode_t mode);
void hal_gpio_write(uint8_t port_idx, uint16_t pin_mask, hal_gpio_status_t status);
hal_gpio_status_t hal_gpio_read(uint8_t port_idx, uint16_t pin_mask);
void hal_gpio_toggle(uint8_t port_idx, uint16_t pin_mask);

// 基于 ID 的高级操作接口
// 使用 uint32_t 作为 id，避免与项目层枚举产生循环依赖
void hal_gpio_id_write(uint32_t id, hal_gpio_status_t status);
void hal_gpio_id_toggle(uint32_t id);
hal_gpio_status_t hal_gpio_id_read(uint32_t id);

#endif // HAL_GPIO_H
