#include "st7789.h"
#include "gpio.h"
#include "spi.h"
#include "timer.h"

void ST7789_Init(void) {
    SPI_Init();
    Timer_SoftDelay(100000);
}
