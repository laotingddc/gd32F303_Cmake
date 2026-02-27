#include "mhal_gpio.h"
#include "gd32f30x.h"
#include <stdint.h>

/* 表驱动映射：port_idx(0~6) <-> GPIOA~GPIOG */
static const uint32_t s_gpio_base_tbl[] = {
    GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG
};

static const rcu_periph_enum s_gpio_rcu_tbl[] = {
    RCU_GPIOA, RCU_GPIOB, RCU_GPIOC, RCU_GPIOD, RCU_GPIOE, RCU_GPIOF, RCU_GPIOG
};

static const uint32_t s_gpio_mode_tbl[] = {
    [MHAL_GPIO_MODE_OUT_PP]      = GPIO_MODE_OUT_PP,
    [MHAL_GPIO_MODE_IN_FLOATING] = GPIO_MODE_IN_FLOATING,
    [MHAL_GPIO_MODE_IN_PU]       = GPIO_MODE_IPU,
    [MHAL_GPIO_MODE_IN_PD]       = GPIO_MODE_IPD,
};

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

static bool _is_valid_port(uint8_t port_idx)
{
    return (port_idx < ARRAY_SIZE(s_gpio_base_tbl));
}

static bool _is_valid_mode(mhal_gpio_mode_t mode)
{
    return ((uint32_t)mode < ARRAY_SIZE(s_gpio_mode_tbl));
}

void mhal_gpio_init(uint8_t port_idx, uint16_t pin_mask, mhal_gpio_mode_t mode) {
    if (!_is_valid_port(port_idx) || !_is_valid_mode(mode) || pin_mask == 0U) {
        return;
    }

    rcu_periph_clock_enable(s_gpio_rcu_tbl[port_idx]);
    gpio_init(s_gpio_base_tbl[port_idx], s_gpio_mode_tbl[mode], GPIO_OSPEED_50MHZ, pin_mask);
}

void mhal_gpio_write(uint8_t port_idx, uint16_t pin_mask, mhal_gpio_status_t status) {
    if (!_is_valid_port(port_idx) || pin_mask == 0U) {
        return;
    }

    gpio_bit_write(s_gpio_base_tbl[port_idx], pin_mask, (bit_status)status);
}

mhal_gpio_status_t mhal_gpio_read(uint8_t port_idx, uint16_t pin_mask) {
    if (!_is_valid_port(port_idx) || pin_mask == 0U) {
        return MHAL_GPIO_LOW;
    }

    return (gpio_input_bit_get(s_gpio_base_tbl[port_idx], pin_mask) == SET) ? MHAL_GPIO_HIGH : MHAL_GPIO_LOW;
}

void mhal_gpio_toggle(uint8_t port_idx, uint16_t pin_mask) {
    if (!_is_valid_port(port_idx) || pin_mask == 0U) {
        return;
    }

    mhal_gpio_status_t status = mhal_gpio_read(port_idx, pin_mask);
    mhal_gpio_write(port_idx, pin_mask, (status == MHAL_GPIO_HIGH) ? MHAL_GPIO_LOW : MHAL_GPIO_HIGH);
}
