#include "device/spi.h"
#include <limits.h>
#include <stdint.h>
#include <stdio.h>

int main() {
    SPI_Init();
    while (1) {
        printf("write data\n");

        for (uint8_t i = 0; i < 16; i++) {
            SPI_Write8bit(i);
        }

        for (volatile int i = 0; i < 1000000; i++)
            ;
    }
    return 0;
}
