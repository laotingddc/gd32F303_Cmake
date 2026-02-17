/**
 * @file rtt_log.h
 * @brief RTT日志封装，支持多级别打印和一键关闭
 */

#ifndef RTT_LOG_H
#define RTT_LOG_H

#include "SEGGER_RTT.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ 配置选项 ============================*/

/* 日志总开关: 注释此行关闭所有日志 */
#define LOG_ENABLE              1

/* 日志级别定义 */
#define LOG_LEVEL_NONE          0   /* 关闭所有日志 */
#define LOG_LEVEL_ERROR         1   /* 仅错误 */
#define LOG_LEVEL_WARN          2   /* 错误 + 警告 */
#define LOG_LEVEL_INFO          3   /* 错误 + 警告 + 信息 */
#define LOG_LEVEL_DEBUG         4   /* 所有日志 */

/* 当前日志级别: 低于此级别的日志不会输出 */
#ifndef LOG_LEVEL
#define LOG_LEVEL               LOG_LEVEL_DEBUG
#endif

/* 是否打印文件名和行号 */
#define LOG_PRINT_FILE_LINE     1

/* RTT Buffer索引 */
#define LOG_RTT_BUFFER          0

/*============================ 颜色定义 ============================*/

#define LOG_COLOR_RESET         RTT_CTRL_RESET
#define LOG_COLOR_ERROR         RTT_CTRL_TEXT_BRIGHT_RED
#define LOG_COLOR_WARN          RTT_CTRL_TEXT_BRIGHT_YELLOW
#define LOG_COLOR_INFO          RTT_CTRL_TEXT_BRIGHT_GREEN
#define LOG_COLOR_DEBUG         RTT_CTRL_TEXT_BRIGHT_CYAN

/*============================ 日志接口 ============================*/

#if defined(LOG_ENABLE) && (LOG_ENABLE == 1)

/* 内部日志函数 */
void _log_output(const char *level, const char *color, const char *file, 
                 int line, const char *fmt, ...);

#if LOG_PRINT_FILE_LINE
    #define _LOG(level, color, fmt, ...) \
        _log_output(level, color, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
    #define _LOG(level, color, fmt, ...) \
        _log_output(level, color, NULL, 0, fmt, ##__VA_ARGS__)
#endif

/* 错误日志 - 红色 */
#if (LOG_LEVEL >= LOG_LEVEL_ERROR)
    #define LOG_E(fmt, ...)     _LOG("E", LOG_COLOR_ERROR, fmt, ##__VA_ARGS__)
#else
    #define LOG_E(fmt, ...)     ((void)0)
#endif

/* 警告日志 - 黄色 */
#if (LOG_LEVEL >= LOG_LEVEL_WARN)
    #define LOG_W(fmt, ...)     _LOG("W", LOG_COLOR_WARN, fmt, ##__VA_ARGS__)
#else
    #define LOG_W(fmt, ...)     ((void)0)
#endif

/* 信息日志 - 绿色 */
#if (LOG_LEVEL >= LOG_LEVEL_INFO)
    #define LOG_I(fmt, ...)     _LOG("I", LOG_COLOR_INFO, fmt, ##__VA_ARGS__)
#else
    #define LOG_I(fmt, ...)     ((void)0)
#endif

/* 调试日志 - 青色 */
#if (LOG_LEVEL >= LOG_LEVEL_DEBUG)
    #define LOG_D(fmt, ...)     _LOG("D", LOG_COLOR_DEBUG, fmt, ##__VA_ARGS__)
#else
    #define LOG_D(fmt, ...)     ((void)0)
#endif

/* 原始输出(无前缀无颜色) */
#define LOG_RAW(fmt, ...)       SEGGER_RTT_printf(LOG_RTT_BUFFER, fmt, ##__VA_ARGS__)

#else /* LOG_ENABLE == 0 */

/* 日志关闭时所有接口为空操作 */
#define LOG_E(fmt, ...)         ((void)0)
#define LOG_W(fmt, ...)         ((void)0)
#define LOG_I(fmt, ...)         ((void)0)
#define LOG_D(fmt, ...)         ((void)0)
#define LOG_RAW(fmt, ...)       ((void)0)

#endif /* LOG_ENABLE */

#ifdef __cplusplus
}
#endif

#endif /* RTT_LOG_H */
