#ifndef _SPI1_H__
#define _SPI1_H__

#include <stdint.h>

typedef struct {
    volatile uint32_t STATUS;  // 0x00: 状态寄存器
    volatile uint32_t CLKDIV;  // 0x04: 时钟分频寄存器
    volatile uint32_t CMD;     // 0x08: 命令寄存器
    volatile uint32_t ADR;     // 0x0C: 地址寄存器
    volatile uint32_t LEN;     // 0x10: 数据长度寄存器
    volatile uint32_t DUM;     // 0x14: Dummy Cycle寄存器
    volatile uint32_t TXFIFO;  // 0x18: 发送FIFO寄存器
    volatile uint32_t _RESERVED; // 0x1C: 预留
    volatile uint32_t RXFIFO;  // 0x20: 接收FIFO寄存器
    volatile uint32_t INTCFG;  // 0x24: 中断配置寄存器
    volatile uint32_t INTSTA;  // 0x28: 中断状态寄存器
} SPI_Regs_t;

#define SPI1 ((SPI_Regs_t *)0x10010000)
#define SPI_DEFAULT_CLOCK_DIVIDER 7

/**
 * @brief 初始化SPI外设和相关GPIO引脚。
 *
 * @param clockDivider SPI时钟分频系数, SCK = APB_CLK / (clockDivider + 1).
 *        可以使用 SPI_DEFAULT_CLOCK_DIVIDER 作为默认值。
 */
void SPI_begin(uint32_t clockDivider);

/**
 * @brief 发送和接收一个8位字节。
 *
 * @param data 要发送的字节。
 * @return uint8_t 从机返回的字节。
 */
uint8_t SPI_transfer(uint8_t data);

/**
 * @brief 发送和接收一个16位字。
 *
 * @param data 要发送的16位数据。
 * @return uint16_t 从机返回的16位数据。
 */
uint16_t SPI_transfer16(uint16_t data);

/**
 * @brief 连续发送和接收一个数据缓冲区。
 * @note 这个函数会就地修改缓冲区内容，用接收到的数据覆盖发送的数据。
 *
 * @param buffer 指向数据缓冲区的指针。
 * @param length 要传输的字节数。
 */
void SPI_transferBytes(uint8_t *buffer, uint16_t length);

/**
 * @brief 仅发送一个数据缓冲区（忽略接收的数据）。
 *
 * @param buffer 指向要发送数据的常量指针。
 * @param length 要发送的字节数。
 */
void SPI_writeBytes(const uint8_t *buffer, uint16_t length);

#endif
