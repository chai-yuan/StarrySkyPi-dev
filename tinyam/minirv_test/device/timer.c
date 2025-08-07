#include "timer.h"

void timer_softDelay(uint32_t cycle) {
    for (volatile int i = 0; i < cycle; i++)
        ;
}
