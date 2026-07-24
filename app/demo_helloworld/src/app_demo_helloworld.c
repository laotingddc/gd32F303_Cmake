#include "app_demo_helloworld.h"

#include "hal_gpio.h"
#include "hal_delay.h"
uint32_t count;
int app_demo_helloworld_init(void)
{
    LOG_I("Demo: Hello World (no queue / no state machine)");

    hal_gpio_id_write(HAL_GPIO_ID_BOARD_LED, HAL_GPIO_LOW);
    hal_gpio_id_write(HAL_GPIO_ID_LED_DATA, HAL_GPIO_LOW);
    hal_gpio_id_write(HAL_GPIO_ID_LED_STATUS, HAL_GPIO_LOW);

    return 0;
}

void app_demo_helloworld_run(void)
{
    uint8_t key_last = (uint8_t)hal_gpio_id_read(HAL_GPIO_ID_KEY_1);

    while (1) {
        uint8_t key_now = (uint8_t)hal_gpio_id_read(HAL_GPIO_ID_KEY_1);
       
        /* 心跳灯：500ms 翻转一次 */
        hal_gpio_id_toggle(HAL_GPIO_ID_BOARD_LED);
        count++;
        LOG_I("Hello World! count = %u", count);
        /* 按键状态透传到状态灯 */
        hal_gpio_id_write(HAL_GPIO_ID_LED_STATUS,
                          key_now ? HAL_GPIO_HIGH : HAL_GPIO_LOW);

        /* 检测按键边沿，在数据灯上给一个提示 */
        if (key_now != key_last) {
            key_last = key_now;
            hal_gpio_id_toggle(HAL_GPIO_ID_LED_DATA);
        }

        hal_delay_ms(500U);
    }
}
