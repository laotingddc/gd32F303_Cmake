#include "rtt_log.h"

#include "SEGGER_RTT.h"

#include <stdbool.h>
#include <stdarg.h>

static bool s_rtt_initialized = false;

static void rtt_log_ensure_init(void)
{
    if (!s_rtt_initialized) {
        SEGGER_RTT_Init();
        s_rtt_initialized = true;
    }
}

void rtt_log_init(void)
{
    rtt_log_ensure_init();
}

unsigned rtt_log_write(unsigned buffer_index, const void *data, unsigned size)
{
    if ((data == NULL) || (size == 0U)) {
        return 0U;
    }

    rtt_log_ensure_init();
    return SEGGER_RTT_Write(buffer_index, data, size);
}

unsigned rtt_log_write_string(unsigned buffer_index, const char *text)
{
    if (text == NULL) {
        return 0U;
    }

    rtt_log_ensure_init();
    return SEGGER_RTT_WriteString(buffer_index, text);
}

int rtt_log_vprintf(unsigned buffer_index, const char *fmt, va_list *args)
{
    if ((fmt == NULL) || (args == NULL)) {
        return -1;
    }

    rtt_log_ensure_init();
    return SEGGER_RTT_vprintf(buffer_index, fmt, args);
}

int rtt_log_printf(unsigned buffer_index, const char *fmt, ...)
{
    int ret;
    va_list args;

    if (fmt == NULL) {
        return -1;
    }

    va_start(args, fmt);
    ret = rtt_log_vprintf(buffer_index, fmt, &args);
    va_end(args);

    return ret;
}
