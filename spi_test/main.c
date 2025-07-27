#include "device/spi.h"
#include "device/timer.h"
#include <limits.h>
#include <stdint.h>
#include <stdio.h>

int main() {
    spi_begin(SPI_DEFAULT_CLOCK_DIVIDER);

    while (1) {
        printf("write data\n");
        uint16_t len = 1024;
        spi_transfer(len & 0xff);
        spi_transfer(len >> 8);
        timer_softDelay(500);

        for (uint32_t i = 0; i < len; i++) {
            spi_transfer(i);
        }

        timer_softDelay(1000000);
    }
    return 0;
}
