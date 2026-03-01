#include "mhal_delay.h"
#include "mhal_gpio.h"
#include "rtt_log.h"
#include "sfud.h"

static void sfud_flash_probe_test(void)
{
    sfud_err err;
    sfud_flash *flash;
    uint8_t rd_buf[16] = {0};

    LOG_I("SFUD test start");

    err = sfud_init();
    if (err != SFUD_SUCCESS) {
        LOG_E("sfud_init failed, err=%d", (int)err);
        return;
    }

    flash = sfud_get_device(SFUD_EXT_FLASH_DEVICE_INDEX);
    if ((flash == NULL) || (!flash->init_ok)) {
        LOG_E("sfud_get_device failed or device not init");
        return;
    }

    LOG_I("Flash name: %s", flash->name ? flash->name : "unknown");
    LOG_I("JEDEC: MID=0x%02X TYPE=0x%02X CAP=0x%02X",
          flash->chip.mf_id,
          flash->chip.type_id,
          flash->chip.capacity_id);
    LOG_I("Capacity: %lu bytes (%lu MB)",
          (unsigned long)flash->chip.capacity,
          (unsigned long)(flash->chip.capacity / (1024UL * 1024UL)));

    err = sfud_read(flash, 0, sizeof(rd_buf), rd_buf);
    if (err != SFUD_SUCCESS) {
        LOG_E("sfud_read(0,16) failed, err=%d", (int)err);
        return;
    }

    LOG_I("Read[0..15]: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
          rd_buf[0], rd_buf[1], rd_buf[2], rd_buf[3],
          rd_buf[4], rd_buf[5], rd_buf[6], rd_buf[7],
          rd_buf[8], rd_buf[9], rd_buf[10], rd_buf[11],
          rd_buf[12], rd_buf[13], rd_buf[14], rd_buf[15]);
}

int main(void)
{
    sfud_flash_probe_test();

    while (1) {
        mhal_gpio_id_toggle(MHAL_GPIO_ID_BOARD_LED);
        mhal_gpio_id_toggle(MHAL_GPIO_ID_LED_DATA);
        mhal_gpio_id_toggle(MHAL_GPIO_ID_LED_STATUS);
        mhal_delay_ms(500);
    }
}
