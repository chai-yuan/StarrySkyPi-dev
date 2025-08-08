#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "port.h"

#define PRINT_SIZE_MAX 512

int printf(const char *fmt, ...) {
    char buf[PRINT_SIZE_MAX];
    va_list ap;
    va_start(ap, fmt);
    int retval = vsprintf(buf, fmt, ap);
    va_end(ap);
    putstr(buf);
    return retval;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
    return vsnprintf(out, PRINT_SIZE_MAX, fmt, ap);
}

int sprintf(char *out, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int result = vsprintf(out, fmt, ap);
    va_end(ap);
    return result;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int result = vsnprintf(out, n, fmt, ap);
    va_end(ap);
    return result;
}

#define NUM_BUFFER_SIZE 32

static char* u_to_str_base(char* buffer_end, unsigned long long num, int base) {
    const char* digits = "0123456789abcdef";
    char* p = buffer_end;
    *p = '\0'; // 从缓冲区末尾设置null终止符
    if (num == 0) {
        *--p = '0';
        return p;
    }
    while (num > 0) {
        *--p = digits[num % base];  // 取余数得到当前位的数字
        num /= base;                // 整除以往前进一位
    }
    return p;
}

static void append_str(char** p_out, size_t* p_remaining, const char* str_to_append, size_t len) {
    if (len >= *p_remaining) {
        len = *p_remaining - 1;
    }
    if (len > 0) {
        memcpy(*p_out, str_to_append, len);
        *p_out += len;
        *p_remaining -= len;
    }
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
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
