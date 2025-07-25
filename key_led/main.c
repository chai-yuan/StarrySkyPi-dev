#include "device/gpio.h"
#include "device/timer.h"

#define LED_PIN 3
#define KEY_PIN 10 // KEY-2

int main(void) {
    gpio_pinMode(GPIO0, LED_PIN, GPIO_MODE_OUTPUT);
    gpio_pinMode(GPIO1, KEY_PIN, GPIO_MODE_INPUT);

    while (1) {
        uint8_t read = gpio_digitalRead(GPIO1, KEY_PIN);
        gpio_digitalWrite(GPIO0, LED_PIN, read);
        timer_softDelay(100000);
    }

    return 0;
}
