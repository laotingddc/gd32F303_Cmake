#include "gd32f30x.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "gd32f307c_eval.h"
#include "auto_init.h"
#include "rtt_log.h"
int cuont = 0;
int main(void)
{
    LOG_I("Hello, GD32F30x! This is a log message with RTT.");
    while (1){
            delay_1ms(1100);
            gpio_bit_write(GPIOA, GPIO_PIN_8, (bit_status)(1-gpio_input_bit_get(GPIOA, GPIO_PIN_8))); 
    }
}

int led_init(void){
    systick_config();
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_8);
}
INIT_BOARD_EXPORT_PRIO(led_init, "02")


