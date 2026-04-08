/*
 * This file is part of the Serial Flash Universal Driver Library.
 *
 * Copyright (c) 2016-2018, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2016-04-23
 */

#include <sfud.h>
#include <stdarg.h>
#include <stdio.h>
#include "hal_spi.h"
#include "hal_delay.h"
#include "rtt_log.h"

static char log_buf[256];

typedef struct {
    hal_spi_id_t spi_id;
} sfud_port_ctx_t;

static sfud_port_ctx_t s_sfud_port_ctx;

static void spi_lock(const sfud_spi *spi)
{
    (void)spi;
}

static void spi_unlock(const sfud_spi *spi)
{
    (void)spi;
}

static void retry_delay_1ms(void)
{
    hal_delay_ms(1U);
}

void sfud_log_debug(const char *file, const long line, const char *format, ...);

/**
 * SPI write data then read data
 */
static sfud_err spi_write_read(const sfud_spi *spi, const uint8_t *write_buf, size_t write_size, uint8_t *read_buf,
        size_t read_size) {
    sfud_port_ctx_t *ctx;
    int ret;

    if ((spi == NULL) || (spi->user_data == NULL)) {
        return SFUD_ERR_READ;
    }

    ctx = (sfud_port_ctx_t *)spi->user_data;

    hal_spi_id_cs_select((uint32_t)ctx->spi_id);

    if ((write_buf != NULL) && (write_size > 0U)) {
        ret = hal_spi_id_transfer_no_cs((uint32_t)ctx->spi_id, write_buf, NULL, write_size);
        if (ret != 0) {
            hal_spi_id_cs_release((uint32_t)ctx->spi_id);
            return SFUD_ERR_WRITE;
        }
    }

    if ((read_buf != NULL) && (read_size > 0U)) {
        ret = hal_spi_id_transfer_no_cs((uint32_t)ctx->spi_id, NULL, read_buf, read_size);
        if (ret != 0) {
            hal_spi_id_cs_release((uint32_t)ctx->spi_id);
            return SFUD_ERR_READ;
        }
    }

    hal_spi_id_cs_release((uint32_t)ctx->spi_id);

    return SFUD_SUCCESS;
}

sfud_err sfud_spi_port_init(sfud_flash *flash) {
    sfud_err result = SFUD_SUCCESS;

    if (flash == NULL) {
        return SFUD_ERR_NOT_FOUND;
    }

    /* 褰撳墠宸ョ▼鍏堝浐瀹氭妸 SFUD 鐨?EXT_FLASH 缁戝畾鍒?hal_spi 鐨?EXT_FLASH */
    s_sfud_port_ctx.spi_id = HAL_SPI_ID_EXT_FLASH;

    flash->spi.wr = spi_write_read;
    flash->spi.lock = spi_lock;
    flash->spi.unlock = spi_unlock;
    flash->spi.user_data = &s_sfud_port_ctx;

    flash->retry.delay = retry_delay_1ms;
    flash->retry.times = 60 * 1000; /* 鏈€闀跨害 60s锛岃鐩栨暣鐗囨摝闄ょ瓑寰?*/

    return result;
}

/**
 * This function is print debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 */
void sfud_log_debug(const char *file, const long line, const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);

    if (file == NULL) {
        file = "?";
    }

    vsnprintf(log_buf, sizeof(log_buf), format, args);
    rtt_log_printf(0U, "[SFUD](%s:%ld) %s", file, line, log_buf);

    va_end(args);
}

/**
 * This function is print routine info.
 *
 * @param format output format
 * @param ... args
 */
void sfud_log_info(const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);

    vsnprintf(log_buf, sizeof(log_buf), format, args);
    rtt_log_printf(0U, "[SFUD] %s", log_buf);

    va_end(args);
}


