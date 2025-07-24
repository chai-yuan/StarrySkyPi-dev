#ifndef _SPI1_H__
#define _SPI1_H__

#include <stdint.h>

void SPI_Init(void);
void SPI_Write8bit(uint8_t byte);
void SPI_Write16bit(uint16_t data);
void SPI_WriteBuffer(const uint8_t *data, uint16_t len);

#endif
