#include "device/gpio.h"
#include "device/timer.h"

#define LED_PIN 3

int main(void) {
    gpio_pinMode(GPIO0, LED_PIN, GPIO_MODE_OUTPUT);

    while (1) {
        gpio_digitalWrite(GPIO0, LED_PIN, HIGH);

        timer_softDelay(1000000);

        gpio_digitalWrite(GPIO0, LED_PIN, LOW);

        timer_softDelay(1000000);
    }

    return 0;
}
