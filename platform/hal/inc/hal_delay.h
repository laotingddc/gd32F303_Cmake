#ifndef HAL_DELAY_H
#define HAL_DELAY_H

#include <stdint.h>

/**
 * @brief 毫秒级延时
 */
void hal_delay_ms(uint32_t ms);

/**
 * @brief 微秒级延时
 */
void hal_delay_us(uint32_t us);

#endif // HAL_DELAY_H
