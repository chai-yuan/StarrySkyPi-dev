#include "spi.h"
#include "gpio.h"
#include <stddef.h>

#define SPI_STATUS_DONE      (1U << 0)  // 传输完成标志
#define SPI_CMD_START_TX     (1U << 1)  // 启动传输/写使能
#define SPI_CMD_CS_ENABLE    (1U << 8)  // 片选使能 (硬件自动控制)
#define SPI_CMD_FIFO_CLEAR   (1U << 4)  // 清空FIFO命令

#define SPI_POLL_MASK 0xFFFF

static inline void spi_waitForTransferDone(void) {
    while ((SPI1->STATUS & SPI_POLL_MASK) != SPI_STATUS_DONE);
}

static inline void spi_setDataLength(uint8_t bits) {
    SPI1->LEN = (uint32_t)bits << 16;
}

void spi_begin(uint32_t clockDivider) {
    // GPIO复用配置：将引脚功能切换到SPI
    gpio_pinMode(GPIO1, 0, GPIO_MODE_AF0);
    gpio_pinMode(GPIO1, 1, GPIO_MODE_AF0);
    gpio_pinMode(GPIO1, 2, GPIO_MODE_AF0);
    gpio_pinMode(GPIO1, 3, GPIO_MODE_AF0);
    gpio_pinMode(GPIO1, 4, GPIO_MODE_AF0);
    gpio_pinMode(GPIO1, 5, GPIO_MODE_AF0);
    // SPI外设配置
    SPI1->STATUS = SPI_CMD_FIFO_CLEAR; // 清空FIFO
    SPI1->STATUS = 0;                  // 清除状态
    SPI1->INTCFG = 0;                  // 禁用到所有中断
    SPI1->DUM    = 0;                  // 无Dummy-Cycle
    SPI1->CLKDIV = clockDivider;       // 设置SPI时钟分频
}

uint8_t spi_transfer(uint8_t data) {
    spi_setDataLength(8);
    SPI1->TXFIFO = (uint32_t)data << 24;
    // 启动单线写传输，并使能片选
    SPI1->STATUS = SPI_CMD_START_TX | SPI_CMD_CS_ENABLE;
    spi_waitForTransferDone();
    return 0;
}

uint16_t spi_transfer16(uint16_t data) {
    spi_setDataLength(16);
    SPI1->TXFIFO = (uint32_t)data << 16;
    SPI1->STATUS = SPI_CMD_START_TX | SPI_CMD_CS_ENABLE;
    spi_waitForTransferDone();
    return 0;
}

void spi_transferBytes(uint8_t *buffer, uint16_t length) {
    if (buffer == NULL || length == 0) {
        return;
    }
    spi_setDataLength(8);
    for (uint16_t i = 0; i < length; ++i) {
        SPI1->TXFIFO = (uint32_t)buffer[i] << 24;
        SPI1->STATUS = SPI_CMD_START_TX | SPI_CMD_CS_ENABLE;
        spi_waitForTransferDone();
        // 读取接收到的字节并覆盖原缓冲区内容
        buffer[i] = (uint8_t)(SPI1->RXFIFO >> 24);
    }
}

void spi_writeBytes(const uint8_t *buffer, uint16_t length) {
    if (buffer == NULL || length == 0) {
        return;
    }
    spi_setDataLength(8);
    for (uint16_t i = 0; i < length; ++i) {
        SPI1->TXFIFO = (uint32_t)buffer[i] << 24;
        SPI1->STATUS = SPI_CMD_START_TX | SPI_CMD_CS_ENABLE;
        spi_waitForTransferDone();
    }
}
