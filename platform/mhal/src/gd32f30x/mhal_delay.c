#include "mhal_delay.h"
#include "systick.h"

void mhal_delay_ms(uint32_t ms) {
    delay_1ms(ms);
}

void mhal_delay_us(uint32_t us) {
    // 暂时由于底层驱动不直接支持，先留空或简单实现
    // 在生产环境建议通过 DWT 或定时器实现
}
