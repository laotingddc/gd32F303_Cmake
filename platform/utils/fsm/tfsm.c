/**
 * @file tfsm.c
 * @brief 表驱动有限状态机库实现
 * @version 1.0.0
 */

#include "tfsm.h"

/*===========================================================================*/
/* 内部宏定义                                                                */
/*===========================================================================*/

#define TFSM_LOG(fsm, level, fmt, ...) \
    do { \
        if ((fsm)->log != NULL) { \
            (fsm)->log(level, fmt, ##__VA_ARGS__); \
        } \
    } while (0)

/*===========================================================================*/
/* API实现                                                                   */
/*===========================================================================*/

/**
 * @brief 初始化状态机实例
 */
int tfsm_init(tfsm_t *fsm, const tfsm_config_t *config)
{
    if (fsm == NULL || config == NULL || config->init_state == NULL) {
        return -1;
    }

    fsm->name = config->name;
    fsm->current = config->init_state;
    fsm->previous = NULL;
    fsm->log = config->log;
    fsm->persist = config->persist;
    fsm->user_data = config->user_data;
    fsm->transition_count = 0;
    fsm->initialized = true;

    /* 执行初始状态的进入动作 */
    if (fsm->current->on_enter != NULL) {
        fsm->current->on_enter(fsm, NULL);
    }

    return 0;
}

/**
 * @brief 反初始化状态机实例
 */
void tfsm_deinit(tfsm_t *fsm)
{
    if (fsm == NULL || !fsm->initialized) {
        return;
    }

    /* 执行当前状态的退出动作 */
    if (fsm->current != NULL && fsm->current->on_exit != NULL) {
        fsm->current->on_exit(fsm, NULL);
    }

    fsm->initialized = false;
    fsm->current = NULL;
    fsm->previous = NULL;
}

/**
 * @brief 处理状态机（检查转换并运行扫描处理器）
 */
int tfsm_process(tfsm_t *fsm, uint32_t event)
{
    if (fsm == NULL || !fsm->initialized || fsm->current == NULL) {
        return -1;
    }

    /* 检查转换条件 */
    const tfsm_transition_t *trans = fsm->current->transitions;
    uint16_t count = fsm->current->transition_count;

    for (uint16_t i = 0; i < count; i++) {
        if (trans[i].guard != NULL && trans[i].guard(fsm, trans[i].user_arg)) {
            /* 执行状态转换 */
            return tfsm_transition_to(fsm, trans[i].target);
        }
    }

    /* 执行扫描处理器 */
    const tfsm_scan_handler_t *handlers = fsm->current->scan_handlers;
    uint16_t handler_count = fsm->current->scan_handler_count;

    for (uint16_t i = 0; i < handler_count; i++) {
        if (handlers[i].handler != NULL && 
            (handlers[i].event_mask == 0 || (handlers[i].event_mask & event))) {
            handlers[i].handler(fsm, event);
        }
    }

    return 0;
}

/**
 * @brief 强制转换到指定状态
 */
int tfsm_transition_to(tfsm_t *fsm, const tfsm_state_t *state)
{
    if (fsm == NULL || !fsm->initialized || state == NULL) {
        return -1;
    }

    /* 执行当前状态的退出动作 */
    if (fsm->current != NULL && fsm->current->on_exit != NULL) {
        fsm->current->on_exit(fsm, (void *)state);
    }

    /* 保存前一个状态 */
    fsm->previous = fsm->current;
    fsm->current = state;
    fsm->transition_count++;

    /* 执行新状态的进入动作 */
    if (fsm->current->on_enter != NULL) {
        fsm->current->on_enter(fsm, (void *)fsm->previous);
    }

    /* 持久化 */
    if (fsm->persist != NULL) {
        fsm->persist(fsm, fsm->current);
    }

    return 1;
}

/**
 * @brief 获取当前状态
 */
const tfsm_state_t *tfsm_get_current(const tfsm_t *fsm)
{
    if (fsm == NULL || !fsm->initialized) {
        return NULL;
    }
    return fsm->current;
}

/**
 * @brief 获取前一个状态
 */
const tfsm_state_t *tfsm_get_previous(const tfsm_t *fsm)
{
    if (fsm == NULL || !fsm->initialized) {
        return NULL;
    }
    return fsm->previous;
}

/**
 * @brief 获取当前状态ID
 */
int32_t tfsm_get_current_id(const tfsm_t *fsm)
{
    if (fsm == NULL || !fsm->initialized || fsm->current == NULL) {
        return -1;
    }
    return fsm->current->id;
}

/**
 * @brief 获取当前状态名称
 */
const char *tfsm_get_current_name(const tfsm_t *fsm)
{
    if (fsm == NULL || !fsm->initialized || fsm->current == NULL) {
        return "NULL";
    }
    return fsm->current->name;
}

/**
 * @brief 检查状态机是否处于指定状态
 */
bool tfsm_is_in_state(const tfsm_t *fsm, const tfsm_state_t *state)
{
    if (fsm == NULL || !fsm->initialized) {
        return false;
    }
    return fsm->current == state;
}

/**
 * @brief 检查状态机是否处于指定ID的状态
 */
bool tfsm_is_in_state_id(const tfsm_t *fsm, int32_t id)
{
    if (fsm == NULL || !fsm->initialized || fsm->current == NULL) {
        return false;
    }
    return fsm->current->id == id;
}

/**
 * @brief 获取用户数据指针
 */
void *tfsm_get_user_data(const tfsm_t *fsm)
{
    if (fsm == NULL) {
        return NULL;
    }
    return fsm->user_data;
}

/**
 * @brief 设置用户数据指针
 */
void tfsm_set_user_data(tfsm_t *fsm, void *data)
{
    if (fsm != NULL) {
        fsm->user_data = data;
    }
}

/**
 * @brief 获取总转换次数
 */
uint32_t tfsm_get_transition_count(const tfsm_t *fsm)
{
    if (fsm == NULL || !fsm->initialized) {
        return 0;
    }
    return fsm->transition_count;
}
