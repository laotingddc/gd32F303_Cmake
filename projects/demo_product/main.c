#include "mhal_delay.h"
#include "mhal_gpio.h"

int main(void)
{
    while (1) {
        mhal_gpio_id_toggle(MHAL_GPIO_ID_BOARD_LED);
        mhal_gpio_id_toggle(MHAL_GPIO_ID_LED_DATA);
        mhal_gpio_id_toggle(MHAL_GPIO_ID_LED_STATUS);
        mhal_delay_ms(200);
    }
}
