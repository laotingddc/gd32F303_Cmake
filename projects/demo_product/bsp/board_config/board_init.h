#ifndef BOARD_INIT_H
#define BOARD_INIT_H

#include <stdint.h>

// 板级引脚定义
#define LED_PORT    0  // GPIOA
#define LED_PIN     (1 << 8)

// 也可以提供简单的宏函数给 App 使用 (可选)
#include "mhal_gpio.h"
#define BOARD_LED_TOGGLE()  mhal_gpio_toggle(LED_PORT, LED_PIN)

#endif // BOARD_INIT_H
