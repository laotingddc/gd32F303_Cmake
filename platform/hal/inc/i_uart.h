#ifndef I_UART_H
#define I_UART_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief UART 硬件接口结构体 (契约定义)
 */
typedef struct {
    int (*Init)(uint32_t baudrate);
    int (*Send)(const uint8_t *data, size_t len);
    int (*Receive)(uint8_t *buf, size_t max_len);
} iUART_Interface_t;

#endif // I_UART_H
