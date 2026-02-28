#include "mhal_delay.h"
#include "mhal_gpio.h"
#include "mhal_spi.h"
#include "rtt_log.h"

#define GD25_CMD_READ_JEDEC_ID   (0x9FU)
#define GD25_CMD_READ_DEVICE_ID  (0x90U)

static int gd25_read_jedec_id(uint8_t *mid, uint8_t *mem_type, uint8_t *cap_code)
{
    uint8_t tx_buf[4] = {GD25_CMD_READ_JEDEC_ID, 0xFFU, 0xFFU, 0xFFU};
    uint8_t rx_buf[4] = {0};
    int ret;

    if ((mid == NULL) || (mem_type == NULL) || (cap_code == NULL)) {
        return -1;
    }

    ret = mhal_spi_id_transfer(MHAL_SPI_ID_EXT_FLASH, tx_buf, rx_buf, sizeof(tx_buf));
    if (ret != 0) {
        return -2;
    }

    *mid = rx_buf[1];
    *mem_type = rx_buf[2];
    *cap_code = rx_buf[3];

    return 0;
}

static int gd25_read_device_id_90(uint8_t *id_msb, uint8_t *id_lsb)
{
    uint8_t tx_buf[6] = {GD25_CMD_READ_DEVICE_ID, 0x00U, 0x00U, 0x00U, 0xFFU, 0xFFU};
    uint8_t rx_buf[6] = {0};
    int ret;

    if ((id_msb == NULL) || (id_lsb == NULL)) {
        return -1;
    }

    ret = mhal_spi_id_transfer(MHAL_SPI_ID_EXT_FLASH, tx_buf, rx_buf, sizeof(tx_buf));
    if (ret != 0) {
        return -2;
    }

    *id_msb = rx_buf[4];
    *id_lsb = rx_buf[5];

    return 0;
}

static uint32_t gd25_capacity_bytes_from_code(uint8_t cap_code)
{
    if ((cap_code >= 3U) && (cap_code <= 31U)) {
        return (uint32_t)(1UL << cap_code);
    }
    return 0U;
}

static const char *gd25_guess_model(uint8_t mid, uint8_t mem_type, uint8_t cap_code)
{
    if (mid != 0xC8U) {
        return "Unknown(non-GD)";
    }

    if ((mem_type != 0x40U) && (mem_type != 0x60U)) {
        return "GD25(unknown series)";
    }

    switch (cap_code) {
        case 0x14: return "GD25Q80";
        case 0x15: return "GD25Q16";
        case 0x16: return "GD25Q32";
        case 0x17: return "GD25Q64";
        case 0x18: return "GD25Q128";
        case 0x19: return "GD25Q256";
        default:   return "GD25Q(unknown density)";
    }
}

static void gd25_print_size(uint32_t bytes)
{
    if (bytes == 0U) {
        LOG_W("Flash size decode failed");
    } else if ((bytes % (1024UL * 1024UL)) == 0U) {
        LOG_I("Flash size: %lu MB (%lu bytes)",
              (unsigned long)(bytes / (1024UL * 1024UL)),
              (unsigned long)bytes);
    } else {
        LOG_I("Flash size: %lu KB (%lu bytes)",
              (unsigned long)(bytes / 1024UL),
              (unsigned long)bytes);
    }
}

int main(void)
{
    uint8_t mid = 0U;
    uint8_t mem_type = 0U;
    uint8_t cap_code = 0U;
    uint8_t did_msb = 0U;
    uint8_t did_lsb = 0U;
    uint32_t flash_size = 0U;
    int ret;

    LOG_I("GD25 SPI1 test start");

    ret = gd25_read_jedec_id(&mid, &mem_type, &cap_code);
    if (ret != 0) {
        LOG_E("Read JEDEC ID failed, ret=%d", ret);
    } else {
        LOG_I("JEDEC ID: MID=0x%02X, TYPE=0x%02X, CAP=0x%02X", mid, mem_type, cap_code);
        LOG_I("Model guess: %s", gd25_guess_model(mid, mem_type, cap_code));

        flash_size = gd25_capacity_bytes_from_code(cap_code);
        gd25_print_size(flash_size);
    }

    ret = gd25_read_device_id_90(&did_msb, &did_lsb);
    if (ret != 0) {
        LOG_W("Read Device ID(0x90) failed, ret=%d", ret);
    } else {
        LOG_I("Device ID(0x90): 0x%02X%02X", did_msb, did_lsb);
    }

    while (1) {
        mhal_gpio_id_toggle(MHAL_GPIO_ID_BOARD_LED);
        mhal_gpio_id_toggle(MHAL_GPIO_ID_LED_DATA);
        mhal_gpio_id_toggle(MHAL_GPIO_ID_LED_STATUS);
        mhal_delay_ms(500);
    }
}
