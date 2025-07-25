#include "MicroCoreEngine/engine/PlatformPorting.h"
#include "device/gpio.h"
#include "device/timer.h"

void game_platform_init();

int main(void) {
    game_platform_init();

    game_init();
    while (1) {
        game_loop();
    }

    return 0;
}
