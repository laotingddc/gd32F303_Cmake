#ifndef MHAL_DELAY_H
#define MHAL_DELAY_H

#include <stdint.h>

/**
 * @brief 毫秒级延时
 */
void mhal_delay_ms(uint32_t ms);

/**
 * @brief 微秒级延时
 */
void mhal_delay_us(uint32_t us);

#endif // MHAL_DELAY_H
