#include "spi.h"
#include "gpio.h"

#define SPI1_BASE_ADDR (uint32_t)0x10010000
#define SPI1_REG_STATUS_OFFSET 0
#define SPI1_REG_CLKDIV_OFFSET 4
#define SPI1_REG_CMD_OFFSET 8
#define SPI1_REG_ADR_OFFSET 12
#define SPI1_REG_LEN_OFFSET 16
#define SPI1_REG_DUM_OFFSET 20
#define SPI1_REG_TXFIFO_OFFSET 24
#define SPI1_REG_RXFIFO_OFFSET 32
#define SPI1_REG_INTCFG_OFFSET 36
#define SPI1_REG_INTSTA_OFFSET 40

#define SPI1_REG_STATUS *((volatile uint32_t *)(SPI1_BASE_ADDR + SPI1_REG_STATUS_OFFSET))
#define SPI1_REG_CLKDIV *((volatile uint32_t *)(SPI1_BASE_ADDR + SPI1_REG_CLKDIV_OFFSET))
#define SPI1_REG_CMD *((volatile uint32_t *)(SPI1_BASE_ADDR + SPI1_REG_CMD_OFFSET))
#define SPI1_REG_ADR *((volatile uint32_t *)(SPI1_BASE_ADDR + SPI1_REG_ADR_OFFSET))
#define SPI1_REG_LEN *((volatile uint32_t *)(SPI1_BASE_ADDR + SPI1_REG_LEN_OFFSET))
#define SPI1_REG_DUM *((volatile uint32_t *)(SPI1_BASE_ADDR + SPI1_REG_DUM_OFFSET))
#define SPI1_REG_TXFIFO *((volatile uint32_t *)(SPI1_BASE_ADDR + SPI1_REG_TXFIFO_OFFSET))
#define SPI1_REG_RXFIFO *((volatile uint32_t *)(SPI1_BASE_ADDR + SPI1_REG_RXFIFO_OFFSET))
#define SPI1_REG_INTCFG *((volatile uint32_t *)(SPI1_BASE_ADDR + SPI1_REG_INTCFG_OFFSET))
#define SPI1_REG_INTSTA *((volatile uint32_t *)(SPI1_BASE_ADDR + SPI1_REG_INTSTA_OFFSET))

#define SPI_CLOCK_DIVIDER 7 // sck = apb_clk/(div+1)
#define SPI_STATUS_DONE 0x1
#define SPI_POLL_MASK 0xFFFF

static inline void SPI_SetDataLen(uint16_t len) { SPI1_REG_LEN = (len << 16); }

void SPI_Init(void) {
    // GPIO复用配置：将引脚功能切换到SPI
    gpio_pinMode(GPIO1, 0, GPIO_MODE_AF0);
    gpio_pinMode(GPIO1, 1, GPIO_MODE_AF0);
    gpio_pinMode(GPIO1, 2, GPIO_MODE_AF0);
    gpio_pinMode(GPIO1, 3, GPIO_MODE_AF0);
    gpio_pinMode(GPIO1, 4, GPIO_MODE_AF0);
    gpio_pinMode(GPIO1, 5, GPIO_MODE_AF0);

    // SPI外设配置
    SPI1_REG_STATUS = (1 << 4); // 清空FIFO
    SPI1_REG_STATUS = 0;
    SPI1_REG_INTCFG = 0;                 // 清空中断配置
    SPI1_REG_DUM    = 0;                 // 清空Dummy-Cycle
    SPI1_REG_CLKDIV = SPI_CLOCK_DIVIDER; // 设置SPI时钟分频
}

void SPI_Write8bit(uint8_t byte) {
    SPI_SetDataLen(8);              // 设置发送字长
    SPI1_REG_TXFIFO = (byte << 24); // 将数据写入发送FIFO
    SPI1_REG_STATUS = 0x0102;       // 片选 开始写传输(单线)
    // 轮询等待传输完成
    while ((SPI1_REG_STATUS & SPI_POLL_MASK) != SPI_STATUS_DONE)
        ;
}

void SPI_Write16bit(uint16_t data) {
    SPI_SetDataLen(16);             // 设置发送字长
    SPI1_REG_TXFIFO = (data << 16); // 将数据写入发送FIFO
    SPI1_REG_STATUS = 0x0102;       // 片选 开始写传输(单线)
    // 轮询等待传输完成
    while ((SPI1_REG_STATUS & SPI_POLL_MASK) != SPI_STATUS_DONE)
        ;
}

void SPI_WriteBuffer(const uint8_t *data, uint16_t len) {
    SPI_SetDataLen(8);
    for (int i = 0; i < len; i++) {
        SPI1_REG_TXFIFO = (data[i] << 24); // 将数据写入发送FIFO
        SPI1_REG_STATUS = 0x0102;          // 片选 开始写传输(单线)
        while ((SPI1_REG_STATUS & SPI_POLL_MASK) != SPI_STATUS_DONE)
            ;
    }
}
