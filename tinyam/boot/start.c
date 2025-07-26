#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
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

BOOT_TEXT static size_t strlen(const char* s) {
    const char* p;
    for (p = s; *p; p++)
        ;
    return p - s;
}

BOOT_TEXT static void* memcpy(void* out, const void* in, size_t n) {
    char* r1 = out;
    const char* r2 = in;

    while (n) {
        *r1++ = *r2++;
        --n;
    }

    return out;
}

#define PRINT_SIZE_MAX 512

#define UART_BASE 0x10000000L
#define UART(offset) (*(volatile char*)((UART_BASE) + (offset)))
#define R_TX 0   // 发送寄存器
#define R_RX 0   // 接收寄存器
#define R_DLL 0  // 除数锁存寄存器低位
#define R_DLM 1  // 除数锁存寄存器高位
#define R_FCR 2  // FIFO控制寄存器
#define R_LCR 3  // 线控寄存器
#define R_LSR 5  // 线状态寄存器

BOOT_TEXT static void putstr(const char *s){
    for(int i=0;s[i];i++){
        while (!(UART(R_LSR) & (1 << 5)))
            ;
        UART(R_TX) = s[i];
    }
}

#define NUM_BUFFER_SIZE 32

BOOT_TEXT static char* u_to_str_base(char* buffer_end, unsigned long long num, int base) {
    const char* digits = "0123456789abcdef";
    char* p = buffer_end;
    *p = '\0'; // 从缓冲区末尾设置null终止符
    if (num == 0) {
        *--p = '0';
        return p;
    }
    while (num > 0) {
        *--p = digits[num % base]; // 取余数得到当前位的数字
        num /= base;              // 整除以往前进一位
    }
    return p;
}

BOOT_TEXT static void append_str(char** p_out, size_t* p_remaining, const char* str_to_append, size_t len) {
    if (len >= *p_remaining) {
        len = *p_remaining - 1;
    }
    if (len > 0) {
        memcpy(*p_out, str_to_append, len);
        *p_out += len;
        *p_remaining -= len;
    }
}

BOOT_TEXT static int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
    if (n == 0) {
        return 0;
    }

    char* p = out;
    const char* fmt_ptr = fmt;
    size_t remaining = n;
    char num_buffer[NUM_BUFFER_SIZE]; // 用于数字转换的通用缓冲区

    while (*fmt_ptr && remaining > 1) {
        if (*fmt_ptr != '%') {
            *p++ = *fmt_ptr++;
            remaining--;
            continue;
        }

        // 处理 '%'
        fmt_ptr++;
        const char* str_to_write = NULL;
        size_t str_len = 0;

        switch (*fmt_ptr) {
            case 'd': {
                int num = va_arg(ap, int);
                char* num_str_start;
                if (num < 0) {
                    // 转换为正数进行处理，注意 INT_MIN 的特殊情况
                    unsigned long long val = -(unsigned long long)num;
                    num_str_start = u_to_str_base(num_buffer + NUM_BUFFER_SIZE - 1, val, 10);
                    *--num_str_start = '-'; // 手动添加负号
                } else {
                    num_str_start = u_to_str_base(num_buffer + NUM_BUFFER_SIZE - 1, num, 10);
                }
                str_to_write = num_str_start;
                break;
            }
            case 'u': {
                unsigned int num = va_arg(ap, unsigned int);
                str_to_write = u_to_str_base(num_buffer + NUM_BUFFER_SIZE - 1, num, 10);
                break;
            }
            case 'x': {
                unsigned int num = va_arg(ap, unsigned int);
                str_to_write = u_to_str_base(num_buffer + NUM_BUFFER_SIZE - 1, num, 16);
                break;
            }
            case 'p': {
                void* ptr = va_arg(ap, void*);
                if (ptr == NULL) {
                    str_to_write = "(nil)";
                } else {
                    // 使用 uintptr_t 来安全地将指针转换为整数
                    uintptr_t ptr_val = (uintptr_t)ptr;
                    char* ptr_str_start = u_to_str_base(num_buffer + NUM_BUFFER_SIZE - 1, ptr_val, 16);
                    // 添加 "0x" 前缀
                    *--ptr_str_start = 'x';
                    *--ptr_str_start = '0';
                    str_to_write = ptr_str_start;
                }
                break;
            }
            case 's': {
                const char* str = va_arg(ap, const char*);
                str_to_write = (str == NULL) ? "(null)" : str;
                break;
            }
            case 'c': {
                // 'char' 在 va_arg 中会被提升为 'int'
                char c = (char)va_arg(ap, int);
                if (remaining > 1) {
                    *p++ = c;
                    remaining--;
                }
                // 'c' 的处理比较特殊，直接写入，不走下面的 append_str 逻辑
                fmt_ptr++;
                continue; // 继续主循环
            }
            case '%': {
                if (remaining > 1) {
                    *p++ = '%';
                    remaining--;
                }
                fmt_ptr++;
                continue; // 继续主循环
            }
            default: {
                // 不支持的格式，原样输出 '%' 和该字符
                if (remaining > 1) {
                    *p++ = '%';
                    remaining--;
                }
                if (remaining > 1) {
                    *p++ = *fmt_ptr;
                    remaining--;
                }
                fmt_ptr++;
                continue; // 继续主循环
            }
        } // end switch

        // 对所有需要写入字符串的格式进行统一处理
        if (str_to_write) {
            str_len = strlen(str_to_write);
            append_str(&p, &remaining, str_to_write, str_len);
        }
        fmt_ptr++;
    }
    *p = '\0'; // 确保字符串以null结尾
    return p - out;
}

BOOT_TEXT static int vsprintf(char *out, const char *fmt, va_list ap) {
    return vsnprintf(out, PRINT_SIZE_MAX, fmt, ap);
}

BOOT_TEXT static int printf(const char *fmt, ...) {
    char buf[PRINT_SIZE_MAX];
    va_list ap;
    va_start(ap, fmt);
    int retval = vsprintf(buf, fmt, ap);
    va_end(ap);
    putstr(buf);
    return retval;
}

BOOT_TEXT int start() {
    printf("===== Smart Bootloader Started (VMA/LMA aware) =====\n\n");

    // --- 1. 复制 .text .data 段 (从 Flash 到 RAM) ---
    size_t data_size = (size_t)(_edata - _sdata);
    printf("Step 1: Preparing to copy .data section.\n");
    printf("  - Source (LMA in Flash): %p\n", _sidata);
    printf("  - Destination (VMA in RAM): %p\n", _sdata);
    printf("  - Size: %u bytes\n", data_size);

    if (data_size > 0) {
        memory_copy(_sdata, _sidata, data_size);
        printf("Step 1: .data copy complete.\n\n");
    } else {
        printf("Step 1: .data section is empty, skipping copy.\n\n");
    }

    // --- 2. 清零 .bss 段 (在 RAM 中) ---
    size_t bss_size = (size_t)(_ebss - _sbss);
    printf("Step 2: Preparing to clear .bss section in RAM.\n");
    printf("  - Start Address (VMA): %p\n", _sbss);
    printf("  - End Address (VMA):   %p\n", _ebss);
    printf("  - Size: %u bytes\n", bss_size);

    if (bss_size > 0) {
        memory_set(_sbss, 0, bss_size);
        printf("Step 2: .bss clearing complete.\n\n");
    } else {
        printf("Step 2: .bss section is empty, skipping clear.\n\n");
    }

    // --- 3. 跳转到应用程序 ---
    printf("Step 3: All preparations complete. Jumping to application at 0x%x...\n", APP_ENTRY_POINT);
    printf("===============================================================\n\n");

    void (*app_entry)(void) = (void (*)(void))APP_ENTRY_POINT;

    app_entry();

    printf("[FATAL ERROR] Application returned to bootloader! Halting.\n");
    while(1);

    return 0;
}
