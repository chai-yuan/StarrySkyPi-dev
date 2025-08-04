#include "MicroCoreEngine/engine/Debug.h"
#include "MicroCoreEngine/engine/PlatformPorting.h"
#include "device/gpio.h"
#include "device/spi.h"
#include "device/timer.h"
#include <stdio.h>

void send_package_buf(const uint8_t *data, uint16_t len, int send_now) {
    #define SEND_BUF_SIZE 2048
    static uint8_t buf[SEND_BUF_SIZE];
    static uint16_t buf_idx = 0;

    if(send_now || (buf_idx + len > SEND_BUF_SIZE)){
        while (gpio_digitalRead(GPIO0, 27) == 0){
            timer_softDelay(1000);
        }

        uint16_t send_len = len + buf_idx;
        spi_transfer(send_len & 0xff); spi_transfer(send_len >> 8);
        timer_softDelay(1000);

        for (int i = 0; i < buf_idx; i++) spi_transfer(buf[i]);
        for (int i = 0; i < len; i++) spi_transfer(data[i]);

        buf_idx = 0;
    } else {
        for (int i = 0; i < len; i++) buf[i+buf_idx] = data[i];
        buf_idx += len;
    }
}

void send_package(const uint8_t *data, uint16_t len){
    while (gpio_digitalRead(GPIO0, 27) == 0){
        timer_softDelay(1000);
    }
    spi_transfer(len & 0xff);
    spi_transfer(len >> 8);
    timer_softDelay(1000);

    for (int i = 0; i < len; i++) {
        spi_transfer(data[i]);
    }
}

