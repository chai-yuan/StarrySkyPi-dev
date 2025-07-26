#include "MicroCoreEngine/engine/PlatformPorting.h"
#include "device/gpio.h"
#include "device/timer.h"
#include <stdio.h>

void game_platform_init();

int main(void) {
    gpio_pinMode(GPIO1, 10, GPIO_MODE_INPUT);
    gpio_pinMode(GPIO1, 7, GPIO_MODE_INPUT);
    game_platform_init();

    game_init();
    while (1) {
        game_loop();
    }

    return 0;
}
