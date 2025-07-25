#include "device/gpio.h"
#include "device/timer.h"
#include <stdio.h>
#include <stdint.h>

// 辅助宏，用于在测试失败时打印信息并暂停
#define CHECK(condition, message) \
    do { \
        if (!(condition)) { \
            printf("FAIL: %s\n", message); \
            while (1); \
        } else { \
            printf("PASS: %s\n", message); \
        } \
    } while (0)

int main(void) {
    // RV64M 边界情况测试

    // --- 除以零测试 ---
    printf("--- Division by Zero Tests ---\n");

    volatile int64_t dividend = 10;
    volatile int64_t divisor_zero = 0;
    volatile int64_t quotient;
    volatile int64_t remainder;

    // DIV: 10 / 0
    asm volatile ("div %0, %1, %2" : "=r"(quotient) : "r"(dividend), "r"(divisor_zero));
    CHECK(quotient == -1, "DIV: Quotient of division by zero should be -1");

    // REM: 10 % 0
    asm volatile ("rem %0, %1, %2" : "=r"(remainder) : "r"(dividend), "r"(divisor_zero));
    CHECK(remainder == dividend, "REM: Remainder of division by zero should be the dividend");

    volatile uint64_t u_dividend = 10;
    volatile uint64_t u_divisor_zero = 0;
    volatile uint64_t u_quotient;
    volatile uint64_t u_remainder;

    // DIVU: 10U / 0U
    asm volatile ("divu %0, %1, %2" : "=r"(u_quotient) : "r"(u_dividend), "r"(u_divisor_zero));
    CHECK(u_quotient == UINT64_MAX, "DIVU: Quotient of unsigned division by zero should be UINT64_MAX");

    // REMU: 10U % 0U
    asm volatile ("remu %0, %1, %2" : "=r"(u_remainder) : "r"(u_dividend), "r"(u_divisor_zero));
    CHECK(u_remainder == u_dividend, "REMU: Remainder of unsigned division by zero should be the dividend");

    volatile int32_t dividend_w = 20;
    volatile int32_t divisor_zero_w = 0;
    volatile int64_t quotient_w;
    volatile int64_t remainder_w;

    // DIVW: 20 / 0
    asm volatile ("divw %0, %1, %2" : "=r"(quotient_w) : "r"(dividend_w), "r"(divisor_zero_w));
    CHECK(quotient_w == -1, "DIVW: Quotient of 32-bit division by zero should be -1");

    // REMW: 20 % 0
    asm volatile ("remw %0, %1, %2" : "=r"(remainder_w) : "r"(dividend_w), "r"(divisor_zero_w));
    CHECK(remainder_w == dividend_w, "REMW: Remainder of 32-bit division by zero should be the dividend");


    // --- 有符号除法溢出测试 ---
    printf("\n--- Signed Division Overflow Tests ---\n");

    volatile int64_t min_int64 = INT64_MIN; // 最小的64位有符号整数
    volatile int64_t minus_one = -1;
    volatile int64_t overflow_quotient;
    volatile int64_t overflow_remainder;

    // DIV: INT64_MIN / -1
    asm volatile ("div %0, %1, %2" : "=r"(overflow_quotient) : "r"(min_int64), "r"(minus_one));
    CHECK(overflow_quotient == min_int64, "DIV Overflow: Quotient should be the dividend (INT64_MIN)");

    // REM: INT64_MIN % -1
    asm volatile ("rem %0, %1, %2" : "=r"(overflow_remainder) : "r"(min_int64), "r"(minus_one));
    CHECK(overflow_remainder == 0, "REM Overflow: Remainder should be 0");

    volatile int32_t min_int32 = INT32_MIN; // 最小的32位有符号整数
    volatile int32_t minus_one_w = -1;
    volatile int64_t overflow_quotient_w;
    volatile int64_t overflow_remainder_w;

    // DIVW: INT32_MIN / -1
    asm volatile ("divw %0, %1, %2" : "=r"(overflow_quotient_w) : "r"(min_int32), "r"(minus_one_w));
    CHECK(overflow_quotient_w == (int64_t)min_int32, "DIVW Overflow: Quotient should be the dividend (INT32_MIN), sign-extended");

    // REMW: INT32_MIN % -1
    asm volatile ("remw %0, %1, %2" : "=r"(overflow_remainder_w) : "r"(min_int32), "r"(minus_one_w));
    CHECK(overflow_remainder_w == 0, "REMW Overflow: Remainder should be 0");


    printf("\nAll boundary condition tests for RV64M completed.\n");
    return 0;
}
