#include "mhal_gpio.h"
#include "auto_init.h"
#include "board_init.h"

/**
 * @brief 板级硬件初始化
 * 这里负责将具体的 MCU 引脚与产品逻辑功能（如 LED）绑定
 */
static int board_hw_init(void) {
    // 初始化 LED：GPIOA, Pin 8
    mhal_gpio_init(LED_PORT, LED_PIN, MHAL_GPIO_MODE_OUT_PP);
    
    // 如果有其他传感器或外设，也在这里初始化
    return 0;
}

// 导出到 BOARD 级自动初始化阶段
INIT_BOARD_EXPORT(board_hw_init);
