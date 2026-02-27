#ifndef MHAL_GPIO_H
#define MHAL_GPIO_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// GPIO 电平状态
typedef enum {
    MHAL_GPIO_LOW = 0,
    MHAL_GPIO_HIGH = 1
} mhal_gpio_status_t;

// GPIO 模式定义
typedef enum {
    MHAL_GPIO_MODE_OUT_PP,
    MHAL_GPIO_MODE_IN_FLOATING,
    MHAL_GPIO_MODE_IN_PU,
    MHAL_GPIO_MODE_IN_PD,
} mhal_gpio_mode_t;

// GPIO 单项配置结构体
typedef struct {
    uint8_t port_idx;
    uint16_t pin_mask;
    mhal_gpio_mode_t mode;
    mhal_gpio_status_t init_level;
} mhal_gpio_cfg_t;

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
#define GPIO_ITEM(name, port, pin, mode, init_level) MHAL_GPIO_ID_##name,
#include "mhal_gpio_table.def"
#undef GPIO_ITEM
    MHAL_GPIO_ID_MAX
} mhal_gpio_id_t;

// 基础物理操作接口
void mhal_gpio_init(uint8_t port_idx, uint16_t pin_mask, mhal_gpio_mode_t mode);
void mhal_gpio_write(uint8_t port_idx, uint16_t pin_mask, mhal_gpio_status_t status);
mhal_gpio_status_t mhal_gpio_read(uint8_t port_idx, uint16_t pin_mask);
void mhal_gpio_toggle(uint8_t port_idx, uint16_t pin_mask);

// 基于 ID 的高级操作接口
// 使用 uint32_t 作为 id，避免与项目层枚举产生循环依赖
void mhal_gpio_id_write(uint32_t id, mhal_gpio_status_t status);
void mhal_gpio_id_toggle(uint32_t id);
mhal_gpio_status_t mhal_gpio_id_read(uint32_t id);

#endif // MHAL_GPIO_H
