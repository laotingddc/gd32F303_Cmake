#ifndef __AUTO_INIT_H__
#define __AUTO_INIT_H__

typedef int (*init_fn_t)(void);

/**
 * @brief 核心导出宏
 * @param level 1-6 (1:Board, 2:Prev, 3:Device, 4:Component, 5:Env, 6:App)
 * @param prio  00-99 (建议使用两位数保证排序正确)
 */
#define INIT_EXPORT(fn, level, prio) \
    __attribute__((used)) const init_fn_t __init_##fn \
    __attribute__((section(".init_fn." level "." prio))) = fn;

/* --- 默认优先级宏 (50) --- */
#define INIT_BOARD_EXPORT(fn)           INIT_EXPORT(fn, "1", "50")
#define INIT_PREV_EXPORT(fn)            INIT_EXPORT(fn, "2", "50")
#define INIT_DEVICE_EXPORT(fn)          INIT_EXPORT(fn, "3", "50")
#define INIT_COMPONENT_EXPORT(fn)       INIT_EXPORT(fn, "4", "50")
#define INIT_ENV_EXPORT(fn)             INIT_EXPORT(fn, "5", "50")
#define INIT_APP_EXPORT(fn)             INIT_EXPORT(fn, "6", "50")

/* --- 自定义优先级宏 --- */
#define INIT_BOARD_EXPORT_PRIO(fn, p)     INIT_EXPORT(fn, "1", p)
#define INIT_PREV_EXPORT_PRIO(fn, p)      INIT_EXPORT(fn, "2", p)
#define INIT_DEVICE_EXPORT_PRIO(fn, p)    INIT_EXPORT(fn, "3", p)
#define INIT_COMPONENT_EXPORT_PRIO(fn, p) INIT_EXPORT(fn, "4", p)
#define INIT_ENV_EXPORT_PRIO(fn, p)       INIT_EXPORT(fn, "5", p)
#define INIT_APP_EXPORT_PRIO(fn, p)       INIT_EXPORT(fn, "6", p)

#endif /* __AUTO_INIT_H__ */
