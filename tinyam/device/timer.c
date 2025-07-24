#include "timer.h"

void Timer_SoftDelay(uint32_t cycle) {
    for (volatile int i = 0; i < cycle; i++)
        ;
}
