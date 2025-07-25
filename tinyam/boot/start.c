#include <stdio.h>
#include <stdint.h>
#include "sections.h"

extern char _sidata[], _sdata[], _edata[], _sbss[], _ebss[];

#define APP_ENTRY_POINT 0x0f000000

BOOT_TEXT static inline void* memory_copy(void* dest, const void* src, size_t n) {
    char* d = dest;
    const char* s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

BOOT_TEXT static inline void* memory_set(void* s, int c, size_t n) {
    unsigned char* p = s;
    while (n--) {
        *p++ = (unsigned char)c;
    }
    return s;
}

BOOT_TEXT int start() {
    // --- 1. 复制 .text .data 段 (从 Flash 到 RAM) ---
    size_t data_size = (size_t)(_edata - _sdata);
    if (data_size > 0) {
        memory_copy(_sdata, _sidata, data_size);
    } else {
    }

    // --- 2. 清零 .bss 段 (在 RAM 中) ---
    size_t bss_size = (size_t)(_ebss - _sbss);

    if (bss_size > 0) {
        memory_set(_sbss, 0, bss_size);
    } else {
    }

    // --- 3. 跳转到应用程序 ---
    void (*app_entry)(void) = (void (*)(void))APP_ENTRY_POINT;

    app_entry();

    while(1);

    return 0;
}
