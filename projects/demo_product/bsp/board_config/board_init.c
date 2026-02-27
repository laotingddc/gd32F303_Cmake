#include "auto_init.h"
#include "board_init.h"
#include "systick.h"

/**
 * @brief 板级硬件初始化
 * 板级基础硬件初始化（GPIO 由 mhal_gpio.c 中 INIT_BOARD_EXPORT_PRIO 自动初始化）
 */
static int board_hw_init(void) {
    systick_config();
    
    return 0;
}

// 导出到 BOARD 级自动初始化阶段
INIT_BOARD_EXPORT(board_hw_init);
