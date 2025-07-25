#include "device/gpio.h"
#include "device/spi.h"
#include "device/timer.h"
#include <limits.h>
#include <stdint.h>
#include <stdio.h>


int main() {
    SPI_begin(SPI_DEFAULT_CLOCK_DIVIDER);

    while (1) {
        printf("write data\n");

        for (uint32_t i = 0; i < 256; i++) {
            SPI_transfer(i);
        }

        timer_softDelay(1000000);
    }
    return 0;
}
