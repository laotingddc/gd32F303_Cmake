#include "mhal_delay.h"
#include "board_init.h"
#include "rtt_log.h"

/**
 * @brief 应用层主循环
 */
int main(void)
{
    LOG_I("Product Demo started.");
    LOG_D("System initialized by auto-init mechanism.");

    while (1) {
        // 使用 BSP 定义的宏，完全屏蔽引脚细节
        BOARD_LED_TOGGLE();
        
        mhal_delay_ms(500);
        LOG_D("Tick");
    }
}
