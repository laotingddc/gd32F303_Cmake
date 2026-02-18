#ifndef GPIO_CFG_H
#define GPIO_CFG_H

#include "mhal_gpio.h"

typedef enum {
    MHAL_GPIO_ID_BOARD_LED,
    MHAL_GPIO_ID_LED_DATA,
    MHAL_GPIO_ID_LED_STATUS,
    MHAL_GPIO_ID_MAX
} mhal_gpio_id_t;

#define BOARD_LED_WRITE(level)  mhal_gpio_id_write(MHAL_GPIO_ID_BOARD_LED, level)
#define BOARD_LED_TOGGLE()      mhal_gpio_id_toggle(MHAL_GPIO_ID_BOARD_LED)
#define BOARD_LED_READ()        mhal_gpio_id_read(MHAL_GPIO_ID_BOARD_LED)
#define LED_DATA_WRITE(level)  mhal_gpio_id_write(MHAL_GPIO_ID_LED_DATA, level)
#define LED_DATA_TOGGLE()      mhal_gpio_id_toggle(MHAL_GPIO_ID_LED_DATA)
#define LED_DATA_READ()        mhal_gpio_id_read(MHAL_GPIO_ID_LED_DATA)
#define LED_STATUS_WRITE(level)  mhal_gpio_id_write(MHAL_GPIO_ID_LED_STATUS, level)
#define LED_STATUS_TOGGLE()      mhal_gpio_id_toggle(MHAL_GPIO_ID_LED_STATUS)
#define LED_STATUS_READ()        mhal_gpio_id_read(MHAL_GPIO_ID_LED_STATUS)

extern const mhal_gpio_cfg_t g_board_gpio_cfg[];
extern const size_t g_board_gpio_cnt;

#endif
