#include "gpio.h"
#include <stddef.h>

void gpio_pinMode(gpio_controller_t *controller, uint8_t pin, GpioPinMode mode) {
    if (controller == NULL || pin >= 32)
        return;

    switch (mode) {
    case GPIO_MODE_INPUT:
        // 1. 设置为软件控制模式
        controller->IOCFG &= ~(1U << pin);
        // 2. 设置为输入方向
        controller->DIR &= ~(1U << pin);
        break;

    case GPIO_MODE_OUTPUT:
        // 1. 设置为软件控制模式
        controller->IOCFG &= ~(1U << pin);
        // 2. 设置为输出方向
        controller->DIR |= (1U << pin);
        break;

    case GPIO_MODE_AF0:
        // 1. 设置为复用功能模式
        controller->IOCFG |= (1U << pin);
        // 2. 选择复用通道0
        controller->PINMUX &= ~(1U << pin);
        break;

    case GPIO_MODE_AF1:
        // 1. 设置为复用功能模式
        controller->IOCFG |= (1U << pin);
        // 2. 选择复用通道1
        controller->PINMUX |= (1U << pin);
        break;
    }
}

void gpio_digitalWrite(gpio_controller_t *controller, uint8_t pin, uint8_t value) {
    if (controller == NULL || pin >= 32)
        return;

    if (value == HIGH) {
        controller->OUT |= (1U << pin);
    } else {
        controller->OUT &= ~(1U << pin);
    }
}

uint8_t gpio_digitalRead(gpio_controller_t *controller, uint8_t pin) {
    if (controller == NULL || pin >= 32)
        return 0;

    return (controller->IN >> pin) & 0x1;
}

void gpio_digitalToggle(gpio_controller_t *controller, uint8_t pin) {
    if (controller == NULL || pin >= 32)
        return;

    controller->OUT ^= (1U << pin);
}

void gpio_configInterrupt(gpio_controller_t *controller, uint8_t pin, GpioIntTrigger trigger);

void gpio_enableInterrupt(gpio_controller_t *controller, uint8_t pin);

void gpio_disableInterrupt(gpio_controller_t *controller, uint8_t pin);

uint8_t gpio_getInterruptStatus(gpio_controller_t *controller, uint8_t pin);
