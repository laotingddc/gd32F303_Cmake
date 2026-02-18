#include "mhal_delay.h"
#include "board_init.h"
#include "rtt_log.h"

int main(void)
{
    LOG_I("Running Multi-LED demo.");
systick_config();
    while (1) {
    BOARD_LED_TOGGLE();
        
        
        LED_DATA_TOGGLE();
       // mhal_delay_ms(200);
        
        LED_STATUS_TOGGLE();
        //mhal_delay_ms(200);
        mhal_delay_ms(1100);
        LOG_D("Blink loop");
    }
}
