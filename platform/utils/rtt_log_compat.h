#ifndef RTT_LOG_COMPAT_H
#define RTT_LOG_COMPAT_H

#include "rtt_log.h"
#include "SEGGER_RTT.h"

#ifndef LOG_ENABLE
#define LOG_ENABLE              1
#endif

#define LOG_LEVEL_NONE          0
#define LOG_LEVEL_ERROR         1
#define LOG_LEVEL_WARN          2
#define LOG_LEVEL_INFO          3
#define LOG_LEVEL_DEBUG         4

#ifndef LOG_LEVEL
#define LOG_LEVEL               LOG_LEVEL_DEBUG
#endif

#ifndef LOG_PRINT_FILE_LINE
#define LOG_PRINT_FILE_LINE     0
#endif

#ifndef LOG_RTT_BUFFER
#define LOG_RTT_BUFFER          0
#endif

#ifndef LOG_COLOR_ERROR
#define LOG_COLOR_ERROR         RTT_CTRL_TEXT_BRIGHT_RED
#endif

#ifndef LOG_COLOR_WARN
#define LOG_COLOR_WARN          RTT_CTRL_TEXT_BRIGHT_YELLOW
#endif

#ifndef LOG_COLOR_INFO
#define LOG_COLOR_INFO          RTT_CTRL_TEXT_BRIGHT_GREEN
#endif

#ifndef LOG_COLOR_DEBUG
#define LOG_COLOR_DEBUG         RTT_CTRL_TEXT_BRIGHT_CYAN
#endif

#ifndef LOG_COLOR_RESET
#define LOG_COLOR_RESET         RTT_CTRL_RESET
#endif

#if defined(LOG_ENABLE) && (LOG_ENABLE == 1)

#if (LOG_LEVEL >= LOG_LEVEL_ERROR)
#if LOG_PRINT_FILE_LINE
#define LOG_E(fmt, ...)         rtt_log_printf(LOG_RTT_BUFFER, "%s[E] (%s:%d) " fmt "%s\r\n", LOG_COLOR_ERROR, __FILE__, __LINE__, ##__VA_ARGS__, LOG_COLOR_RESET)
#else
#define LOG_E(fmt, ...)         rtt_log_printf(LOG_RTT_BUFFER, "%s[E] " fmt "%s\r\n", LOG_COLOR_ERROR, ##__VA_ARGS__, LOG_COLOR_RESET)
#endif
#else
#define LOG_E(fmt, ...)         ((void)0)
#endif

#if (LOG_LEVEL >= LOG_LEVEL_WARN)
#if LOG_PRINT_FILE_LINE
#define LOG_W(fmt, ...)         rtt_log_printf(LOG_RTT_BUFFER, "%s[W] (%s:%d) " fmt "%s\r\n", LOG_COLOR_WARN, __FILE__, __LINE__, ##__VA_ARGS__, LOG_COLOR_RESET)
#else
#define LOG_W(fmt, ...)         rtt_log_printf(LOG_RTT_BUFFER, "%s[W] " fmt "%s\r\n", LOG_COLOR_WARN, ##__VA_ARGS__, LOG_COLOR_RESET)
#endif
#else
#define LOG_W(fmt, ...)         ((void)0)
#endif

#if (LOG_LEVEL >= LOG_LEVEL_INFO)
#if LOG_PRINT_FILE_LINE
#define LOG_I(fmt, ...)         rtt_log_printf(LOG_RTT_BUFFER, "%s[I] (%s:%d) " fmt "%s\r\n", LOG_COLOR_INFO, __FILE__, __LINE__, ##__VA_ARGS__, LOG_COLOR_RESET)
#else
#define LOG_I(fmt, ...)         rtt_log_printf(LOG_RTT_BUFFER, "%s[I] " fmt "%s\r\n", LOG_COLOR_INFO, ##__VA_ARGS__, LOG_COLOR_RESET)
#endif
#else
#define LOG_I(fmt, ...)         ((void)0)
#endif

#if (LOG_LEVEL >= LOG_LEVEL_DEBUG)
#if LOG_PRINT_FILE_LINE
#define LOG_D(fmt, ...)         rtt_log_printf(LOG_RTT_BUFFER, "%s[D] (%s:%d) " fmt "%s\r\n", LOG_COLOR_DEBUG, __FILE__, __LINE__, ##__VA_ARGS__, LOG_COLOR_RESET)
#else
#define LOG_D(fmt, ...)         rtt_log_printf(LOG_RTT_BUFFER, "%s[D] " fmt "%s\r\n", LOG_COLOR_DEBUG, ##__VA_ARGS__, LOG_COLOR_RESET)
#endif
#else
#define LOG_D(fmt, ...)         ((void)0)
#endif

#define LOG_RAW(fmt, ...)       rtt_log_printf(LOG_RTT_BUFFER, fmt, ##__VA_ARGS__)

#else

#define LOG_E(fmt, ...)         ((void)0)
#define LOG_W(fmt, ...)         ((void)0)
#define LOG_I(fmt, ...)         ((void)0)
#define LOG_D(fmt, ...)         ((void)0)
#define LOG_RAW(fmt, ...)       ((void)0)

#endif

#endif /* RTT_LOG_COMPAT_H */
