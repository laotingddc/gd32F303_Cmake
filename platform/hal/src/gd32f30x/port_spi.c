#include "hal_spi.h"
#include "gd32f30x.h"

#define SPI_WAIT_TIMEOUT (1000000UL)

static uint32_t _to_spi_periph(hal_spi_bus_t bus)
{
    switch (bus) {
        case HAL_SPI_BUS_1: return SPI1;
        case HAL_SPI_BUS_2: return SPI2;
        default: return 0U;
    }
}

static uint32_t _to_spi_ck_mode(hal_spi_mode_t mode)
{
    switch (mode) {
        case HAL_SPI_MODE_0: return SPI_CK_PL_LOW_PH_1EDGE;
        case HAL_SPI_MODE_1: return SPI_CK_PL_LOW_PH_2EDGE;
        case HAL_SPI_MODE_2: return SPI_CK_PL_HIGH_PH_1EDGE;
        case HAL_SPI_MODE_3: return SPI_CK_PL_HIGH_PH_2EDGE;
        default: return SPI_CK_PL_LOW_PH_1EDGE;
    }
}

static uint32_t _to_spi_prescaler(hal_spi_baud_div_t baud_div)
{
    switch (baud_div) {
        case HAL_SPI_BAUD_DIV_2:   return SPI_PSC_2;
        case HAL_SPI_BAUD_DIV_4:   return SPI_PSC_4;
        case HAL_SPI_BAUD_DIV_8:   return SPI_PSC_8;
        case HAL_SPI_BAUD_DIV_16:  return SPI_PSC_16;
        case HAL_SPI_BAUD_DIV_32:  return SPI_PSC_32;
        case HAL_SPI_BAUD_DIV_64:  return SPI_PSC_64;
        case HAL_SPI_BAUD_DIV_128: return SPI_PSC_128;
        case HAL_SPI_BAUD_DIV_256: return SPI_PSC_256;
        default: return SPI_PSC_8;
    }
}

static void _spi_bus_pin_init(hal_spi_bus_t bus)
{
    if (bus == HAL_SPI_BUS_1) {
        /* SPI1 固定引脚：PB13(SCK), PB14(MISO), PB15(MOSI) */
        rcu_periph_clock_enable(RCU_GPIOB);
        rcu_periph_clock_enable(RCU_SPI1);

        gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_15);
        gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
        return;
    }

    if (bus == HAL_SPI_BUS_2) {
        /* SPI2 采用重映射到 PC10/PC11/PC12，避免占用调试口相关引脚 */
        rcu_periph_clock_enable(RCU_AF);
        rcu_periph_clock_enable(RCU_GPIOC);
        rcu_periph_clock_enable(RCU_SPI2);

        gpio_pin_remap_config(GPIO_SPI2_REMAP, ENABLE);
        gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10 | GPIO_PIN_12);
        gpio_init(GPIOC, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
    }
}

int hal_spi_port_bus_init(hal_spi_bus_t bus, hal_spi_mode_t mode, hal_spi_baud_div_t baud_div)
{
    uint32_t spi_periph = _to_spi_periph(bus);
    if (spi_periph == 0U) {
        return -1;
    }

    _spi_bus_pin_init(bus);

    spi_parameter_struct spi_init_struct;
    spi_struct_para_init(&spi_init_struct);

    spi_init_struct.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode = SPI_MASTER;
    spi_init_struct.frame_size = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = _to_spi_ck_mode(mode);
    spi_init_struct.nss = SPI_NSS_SOFT;
    spi_init_struct.prescale = _to_spi_prescaler(baud_div);
    spi_init_struct.endian = SPI_ENDIAN_MSB;

    spi_init(spi_periph, &spi_init_struct);
    spi_enable(spi_periph);

    return 0;
}

int hal_spi_port_transfer_byte(hal_spi_bus_t bus, uint8_t tx, uint8_t *rx)
{
    uint32_t spi_periph = _to_spi_periph(bus);
    uint32_t timeout = SPI_WAIT_TIMEOUT;
    uint16_t rx_val;

    if (spi_periph == 0U) {
        return -1;
    }

    while ((RESET == spi_i2s_flag_get(spi_periph, SPI_FLAG_TBE)) && (timeout > 0U)) {
        timeout--;
    }
    if (timeout == 0U) {
        return -2;
    }

    spi_i2s_data_transmit(spi_periph, tx);

    timeout = SPI_WAIT_TIMEOUT;
    while ((RESET == spi_i2s_flag_get(spi_periph, SPI_FLAG_RBNE)) && (timeout > 0U)) {
        timeout--;
    }
    if (timeout == 0U) {
        return -3;
    }

    rx_val = spi_i2s_data_receive(spi_periph);
    if (rx != NULL) {
        *rx = (uint8_t)(rx_val & 0xFFU);
    }

    return 0;
}
