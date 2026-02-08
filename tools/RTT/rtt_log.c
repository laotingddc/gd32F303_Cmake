/**
 * @file rtt_log.c
 * @brief RTT日志实现
 */

#include "rtt_log.h"

#if defined(LOG_ENABLE) && (LOG_ENABLE == 1)

#include <stdarg.h>
#include <string.h>

/**
 * @brief 从完整路径中提取文件名
 */
static const char* _extract_filename(const char *path)
{
    if (path == NULL) return "";
    
    const char *name = path;
    while (*path) {
        if (*path == '/' || *path == '\\') {
            name = path + 1;
        }
        path++;
    }
    return name;
}

/**
 * @brief 日志输出函数
 */
void _log_output(const char *level, const char *color, const char *file, 
                 int line, const char *fmt, ...)
{
    va_list args;
    
    /* 输出颜色和级别标签 */
    SEGGER_RTT_WriteString(LOG_RTT_BUFFER, color);
    SEGGER_RTT_printf(LOG_RTT_BUFFER, "[%s] ", level);
    
    /* 输出文件名和行号 */
    if (file != NULL) {
        const char *filename = _extract_filename(file);
        SEGGER_RTT_printf(LOG_RTT_BUFFER, "(%s:%d) ", filename, line);
    }
    
    /* 输出用户消息 */
    va_start(args, fmt);
    SEGGER_RTT_vprintf(LOG_RTT_BUFFER, fmt, &args);
    va_end(args);
    
    /* 重置颜色并换行 */
    SEGGER_RTT_WriteString(LOG_RTT_BUFFER, LOG_COLOR_RESET);
    SEGGER_RTT_WriteString(LOG_RTT_BUFFER, "\r\n");
}

#endif /* LOG_ENABLE */
