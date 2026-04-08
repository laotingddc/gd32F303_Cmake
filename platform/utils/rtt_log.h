#ifndef RTT_LOG_H
#define RTT_LOG_H

#include <stdarg.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void rtt_log_init(void);
unsigned rtt_log_write(unsigned buffer_index, const void *data, unsigned size);
unsigned rtt_log_write_string(unsigned buffer_index, const char *text);
int rtt_log_vprintf(unsigned buffer_index, const char *fmt, va_list *args);
int rtt_log_printf(unsigned buffer_index, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* RTT_LOG_H */
