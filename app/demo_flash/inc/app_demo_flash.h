#ifndef APP_DEMO_FLASH_H
#define APP_DEMO_FLASH_H
#include "rtt_log_compat.h"

/*
 * demo_flash 应用入口：
 * - app_demo_flash_init(): 初始化与一次性自检
 * - app_demo_flash_run(): 主循环（不返回）
 */
void app_demo_flash_init(void);
void app_demo_flash_run(void);

#endif /* APP_DEMO_FLASH_H */
