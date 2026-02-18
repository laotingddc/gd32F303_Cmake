#include "mhal_gpio.h"
#include "auto_init.h"
#include "board_init.h"
#include "gpio_cfg.h"

/**
 * @brief 板级硬件初始化
 * 现在通过加载 JSON 生成的配置数组来初始化所有 GPIO
 */
static int board_hw_init(void) {
    // 自动加载所有在 JSON 中定义的 GPIO
    mhal_gpio_load_cfg(g_board_gpio_cfg, g_board_gpio_cnt);
    
    return 0;
}

// 导出到 BOARD 级自动初始化阶段
INIT_BOARD_EXPORT(board_hw_init);
