#include "hal_gpio.h"
#include "hal_delay.h"
#include "rtt_log_compat.h"

int main(void)
{
    LOG_I("Hello World!");

    while (1) {
        hal_gpio_id_toggle(HAL_GPIO_ID_BOARD_LED);
        hal_gpio_id_toggle(HAL_GPIO_ID_LED_DATA);
        hal_gpio_id_toggle(HAL_GPIO_ID_LED_STATUS);
        hal_delay_ms(500);
    }

    return 0;
}
