#include "gpio_cfg.h"

const mhal_gpio_cfg_t g_board_gpio_cfg[] = {
    [MHAL_GPIO_ID_BOARD_LED] = { 0, 256, MHAL_GPIO_MODE_OUT_PP, MHAL_GPIO_LOW },
    [MHAL_GPIO_ID_LED_DATA] = { 4, 64, MHAL_GPIO_MODE_OUT_PP, MHAL_GPIO_LOW },
    [MHAL_GPIO_ID_LED_STATUS] = { 5, 64, MHAL_GPIO_MODE_OUT_PP, MHAL_GPIO_LOW },
};

const size_t g_board_gpio_cnt = MHAL_GPIO_ID_MAX;
