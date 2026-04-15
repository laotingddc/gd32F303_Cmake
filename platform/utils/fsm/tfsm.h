/**
 * @file tfsm.h
 * @brief 表驱动有限状态机库
 * @version 1.0.0
 * @date 2026-01-09
 * 
 * 轻量级、工业级、表驱动的有限状态机库
 * 特性：
 *   - 零外部依赖（纯C99）
 *   - 多实例支持（无全局状态）
 *   - 进入/退出动作带上下文传递
 *   - 转换守卫条件
 *   - 事件驱动和轮询模式
 *   - 可配置的日志和持久化回调
 *   - 线程安全设计（用户提供锁）
 */

#ifndef __TFSM_H__
#define __TFSM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*===========================================================================*/
/* 版本信息                                                                  */
/*===========================================================================*/
#define TFSM_VERSION_MAJOR  1
#define TFSM_VERSION_MINOR  0
#define TFSM_VERSION_PATCH  0

/*===========================================================================*/
/* 配置选项                                                                  */
/*===========================================================================*/
#ifndef TFSM_MAX_NAME_LEN
#define TFSM_MAX_NAME_LEN   32
#endif

#ifndef TFSM_ASSERT
#define TFSM_ASSERT(expr)   ((void)0)
#endif

/*===========================================================================*/
/* 类型定义                                                                  */
/*===========================================================================*/

/* 前向声明 */
typedef struct tfsm_state_s tfsm_state_t;
typedef struct tfsm_s tfsm_t;

/**
 * @brief 动作回调函数类型
 * @param fsm   状态机实例指针
 * @param arg   上下文参数（退出/进入动作的前一个/下一个状态）
 * @return      成功返回0，错误返回负数
 */
typedef int32_t (*tfsm_action_fn)(tfsm_t *fsm, void *arg);

/**
 * @brief 守卫/检查函数类型，用于转换条件判断
 * @param fsm   状态机实例指针
 * @param arg   用户自定义参数
 * @return      返回true则执行转换，返回false则不转换
 */
typedef bool (*tfsm_guard_fn)(tfsm_t *fsm, void *arg);

/**
 * @brief 扫描/运行函数类型，用于周期性动作
 * @param fsm   状态机实例指针
 * @param event 当前事件掩码
 */
typedef void (*tfsm_scan_fn)(tfsm_t *fsm, uint32_t event);

/**
 * @brief 日志回调函数类型
 * @param level 日志级别（0=错误, 1=警告, 2=信息, 3=调试）
 * @param fmt   格式化字符串
 * @param ...   可变参数
 */
typedef void (*tfsm_log_fn)(int level, const char *fmt, ...);

/**
 * @brief 状态持久化回调函数类型
 * @param fsm   状态机实例指针
 * @param state 要持久化的状态指针
 */
typedef void (*tfsm_persist_fn)(tfsm_t *fsm, const tfsm_state_t *state);

/*===========================================================================*/
/* 转换定义                                                                  */
/*===========================================================================*/

/**
 * @brief 状态转换定义结构体
 */
typedef struct {
    tfsm_guard_fn guard;            /**< 守卫函数（条件检查） */
    const tfsm_state_t *target;     /**< 目标状态 */
    void *user_arg;                 /**< 传递给守卫函数的用户参数 */
} tfsm_transition_t;

/**
 * @brief 定义转换的辅助宏
 */
#define TFSM_TRANSITION(guard_fn, target_state) \
    { .guard = (guard_fn), .target = (target_state), .user_arg = NULL }

#define TFSM_TRANSITION_EX(guard_fn, target_state, arg) \
    { .guard = (guard_fn), .target = (target_state), .user_arg = (arg) }

/*===========================================================================*/
/* 扫描处理器定义                                                            */
/*===========================================================================*/

/**
 * @brief 扫描处理器定义，用于周期性动作
 */
typedef struct {
    tfsm_scan_fn handler;           /**< 扫描函数 */
    uint32_t event_mask;            /**< 触发此处理器的事件掩码 */
} tfsm_scan_handler_t;

/**
 * @brief 定义扫描处理器的辅助宏
 */
#define TFSM_SCAN_HANDLER(fn, mask) \
    { .handler = (fn), .event_mask = (mask) }

/*===========================================================================*/
/* 状态定义                                                                  */
/*===========================================================================*/

/**
 * @brief 状态定义结构体
 */
struct tfsm_state_s {
    const char *name;                           /**< 状态名称（用于调试） */
    int32_t id;                                 /**< 状态ID（用户定义） */
    tfsm_action_fn on_enter;                    /**< 进入动作回调 */
    tfsm_action_fn on_exit;                     /**< 退出动作回调 */
    const tfsm_transition_t *transitions;       /**< 转换表 */
    uint16_t transition_count;                  /**< 转换数量 */
    const tfsm_scan_handler_t *scan_handlers;   /**< 扫描处理器表 */
    uint16_t scan_handler_count;                /**< 扫描处理器数量 */
};

/**
 * @brief 定义状态的辅助宏
 */
#define TFSM_STATE_DEF(state_name, state_id, enter_fn, exit_fn, trans_arr, scan_arr) \
    { \
        .name = (state_name), \
        .id = (state_id), \
        .on_enter = (enter_fn), \
        .on_exit = (exit_fn), \
        .transitions = (trans_arr), \
        .transition_count = (trans_arr) ? (sizeof(trans_arr) / sizeof((trans_arr)[0])) : 0, \
        .scan_handlers = (scan_arr), \
        .scan_handler_count = (scan_arr) ? (sizeof(scan_arr) / sizeof((scan_arr)[0])) : 0, \
    }

/**
 * @brief 获取数组大小的辅助宏
 */
#define TFSM_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/*===========================================================================*/
/* 状态机实例                                                                */
/*===========================================================================*/

/**
 * @brief 状态机配置结构体
 */
typedef struct {
    const char *name;               /**< 状态机名称 */
    const tfsm_state_t *init_state; /**< 初始状态 */
    tfsm_log_fn log;                /**< 日志回调（可选） */
    tfsm_persist_fn persist;        /**< 持久化回调（可选） */
    void *user_data;                /**< 用户数据指针 */
} tfsm_config_t;

/**
 * @brief 状态机实例结构体
 */
struct tfsm_s {
    const char *name;                   /**< 状态机名称 */
    const tfsm_state_t *current;        /**< 当前状态 */
    const tfsm_state_t *previous;       /**< 前一个状态 */
    tfsm_log_fn log;                    /**< 日志回调 */
    tfsm_persist_fn persist;            /**< 持久化回调 */
    void *user_data;                    /**< 用户数据指针 */
    uint32_t transition_count;          /**< 总转换次数 */
    bool initialized;                   /**< 初始化标志 */
};

/*===========================================================================*/
/* 日志级别                                                                  */
/*===========================================================================*/
#define TFSM_LOG_ERROR  0
#define TFSM_LOG_WARN   1
#define TFSM_LOG_INFO   2
#define TFSM_LOG_DEBUG  3

/*===========================================================================*/
/* API函数                                                                   */
/*===========================================================================*/

/**
 * @brief 初始化状态机实例
 * @param fsm    状态机实例指针
 * @param config 配置结构体
 * @return       成功返回0，错误返回负数
 */
int tfsm_init(tfsm_t *fsm, const tfsm_config_t *config);

/**
 * @brief 反初始化状态机实例
 * @param fsm    状态机实例指针
 */
void tfsm_deinit(tfsm_t *fsm);

/**
 * @brief 处理状态机（检查转换并运行扫描处理器）
 * @param fsm    状态机实例指针
 * @param event  事件掩码（轮询模式传0）
 * @return       成功返回0，发生转换返回正数，错误返回负数
 */
int tfsm_process(tfsm_t *fsm, uint32_t event);

/**
 * @brief 强制转换到指定状态
 * @param fsm    状态机实例指针
 * @param state  目标状态
 * @return       成功返回0，错误返回负数
 */
int tfsm_transition_to(tfsm_t *fsm, const tfsm_state_t *state);

/**
 * @brief 获取当前状态
 * @param fsm    状态机实例指针
 * @return       当前状态指针，未初始化返回NULL
 */
const tfsm_state_t *tfsm_get_current(const tfsm_t *fsm);

/**
 * @brief 获取前一个状态
 * @param fsm    状态机实例指针
 * @return       前一个状态指针，没有则返回NULL
 */
const tfsm_state_t *tfsm_get_previous(const tfsm_t *fsm);

/**
 * @brief 获取当前状态ID
 * @param fsm    状态机实例指针
 * @return       当前状态ID，未初始化返回-1
 */
int32_t tfsm_get_current_id(const tfsm_t *fsm);

/**
 * @brief 获取当前状态名称
 * @param fsm    状态机实例指针
 * @return       当前状态名称，未初始化返回"NULL"
 */
const char *tfsm_get_current_name(const tfsm_t *fsm);

/**
 * @brief 检查状态机是否处于指定状态
 * @param fsm    状态机实例指针
 * @param state  要检查的状态
 * @return       处于指定状态返回true
 */
bool tfsm_is_in_state(const tfsm_t *fsm, const tfsm_state_t *state);

/**
 * @brief 检查状态机是否处于指定ID的状态
 * @param fsm    状态机实例指针
 * @param id     要检查的状态ID
 * @return       处于指定状态返回true
 */
bool tfsm_is_in_state_id(const tfsm_t *fsm, int32_t id);

/**
 * @brief 获取用户数据指针
 * @param fsm    状态机实例指针
 * @return       用户数据指针
 */
void *tfsm_get_user_data(const tfsm_t *fsm);

/**
 * @brief 设置用户数据指针
 * @param fsm    状态机实例指针
 * @param data   用户数据指针
 */
void tfsm_set_user_data(tfsm_t *fsm, void *data);

/**
 * @brief 获取总转换次数
 * @param fsm    状态机实例指针
 * @return       自初始化以来的总转换次数
 */
uint32_t tfsm_get_transition_count(const tfsm_t *fsm);

/*===========================================================================*/
/* 事件辅助宏                                                                */
/*===========================================================================*/

/**
 * @brief 从事件ID创建事件掩码
 */
#define TFSM_EVENT(id)  (1U << (id))

#ifdef __cplusplus
}
#endif

#endif /* __TFSM_H__ */
