#ifndef _GPIO_H__
#define _GPIO_H__

#include <stdint.h>

/**
 * @brief GPIO控制器寄存器映射结构体
 * @note 成员必须严格按照硬件手册中的内存地址顺序排列。
 *       'volatile'关键字是必须的，防止编译器优化掉对硬件寄存器的读写。
 */
typedef struct {
    volatile uint32_t PADDIR;   // 0x00: PAD Direction Register
    volatile uint32_t PADIN;    // 0x04: PAD Data In Register
    volatile uint32_t PADOUT;   // 0x08: PAD Data Out Register
    volatile uint32_t INTEN;    // 0x0C: Interrupt Enable Register
    volatile uint32_t INTTYPE0; // 0x10: Interrupt Type0 Register
    volatile uint32_t INTTYPE1; // 0x14: Interrupt Type1 Register
    volatile uint32_t INTSTAT;  // 0x18: Interrupt State Register
    volatile uint32_t IOCFG;    // 0x1C: IO Config Register
    volatile uint32_t PINMUX;   // 0x20: Pin Mux Register
} gpio_controller_t;

#define GPIO0 ((gpio_controller_t *)0x10002000)
#define GPIO1 ((gpio_controller_t *)0x10003000)

#ifndef HIGH
#define HIGH 1
#endif
#ifndef LOW
#define LOW 0
#endif

/**
 * @brief GPIO引脚模式
 */
typedef enum {
    GPIO_MODE_INPUT,  // 软件控制输入模式
    GPIO_MODE_OUTPUT, // 软件控制输出模式
    GPIO_MODE_AF0,    // 复用功能0 (Alternate Function 0)
    GPIO_MODE_AF1     // 复用功能1 (Alternate Function 1)
} GpioPinMode;

/**
 * @brief GPIO中断触发类型
 */
typedef enum {
    GPIO_TRIGGER_HIGH_LEVEL,  // 高电平触发
    GPIO_TRIGGER_LOW_LEVEL,   // 低电平触发
    GPIO_TRIGGER_RISING_EDGE, // 上升沿触发
    GPIO_TRIGGER_FALLING_EDGE // 下降沿触发
} GpioIntTrigger;

/**
 * @brief 设置指定GPIO引脚的模式
 * @param controller 指向GPIO控制器的指针 (例如 GPIO0, GPIO1)
 * @param pin        引脚号 (0 to GPIO_NUM-1)
 * @param mode       引脚模式 (GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF0, GPIO_MODE_AF1)
 */
void gpio_pinMode(gpio_controller_t *controller, uint8_t pin, GpioPinMode mode);

/**
 * @brief 设置GPIO输出引脚的电平
 * @param controller 指向GPIO控制器的指针
 * @param pin        引脚号
 * @param value      电平值 (HIGH 或 LOW)
 */
void gpio_digitalWrite(gpio_controller_t *controller, uint8_t pin, uint8_t value);

/**
 * @brief 读取GPIO输入引脚的电平
 * @param controller 指向GPIO控制器的指针
 * @param pin        引脚号
 * @return           当前引脚电平 (HIGH 或 LOW)
 */

uint8_t gpio_digitalRead(gpio_controller_t *controller, uint8_t pin);

/**
 * @brief 翻转GPIO输出引脚的电平
 * @param controller 指向GPIO控制器的指针
 * @param pin        引脚号
 */
void gpio_digitalToggle(gpio_controller_t *controller, uint8_t pin);

/**
 * @brief 配置GPIO引脚的中断
 * @param controller 指向GPIO控制器的指针
 * @param pin        引脚号
 * @param trigger    中断触发方式
 */
void gpio_configInterrupt(gpio_controller_t *controller, uint8_t pin, GpioIntTrigger trigger);

/**
 * @brief 使能指定引脚的中断
 * @param controller 指向GPIO控制器的指针
 * @param pin        引脚号
 */
void gpio_enableInterrupt(gpio_controller_t *controller, uint8_t pin);

/**
 * @brief 禁能指定引脚的中断
 * @param controller 指向GPIO控制器的指针
 * @param pin        引脚号
 */
void gpio_disableInterrupt(gpio_controller_t *controller, uint8_t pin);

/**
 * @brief 获取指定引脚的中断状态
 * @param controller 指向GPIO控制器的指针
 * @param pin        引脚号
 * @return           如果中断发生则返回1，否则返回0
 * @note 手册中未说明如何清除INTSTAT位，通常是写1清除或在中断服务程序中自动清除。
 *       这部分需要参考CPU中断控制器（如NVIC）和更详细的手册。
 */
uint8_t gpio_getInterruptStatus(gpio_controller_t *controller, uint8_t pin);

#endif
