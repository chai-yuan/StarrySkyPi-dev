#include <stdio.h>
#include <stdint.h>
#include <limits.h>

#define TIMER_0_BASE_ADDR       (uint32_t)0x10009000
#define TIMER_0_REG_CTRL_OFFSET 0
#define TIMER_0_REG_PSCR_OFFSET 4
#define TIMER_0_REG_CNT_OFFSET  8
#define TIMER_0_REG_CMP_OFFSET  12
#define TIMER_0_REG_STAT_OFFSET 16

#define TIMER_0_REG_CTRL *((volatile uint32_t *)(TIMER_0_BASE_ADDR + TIMER_0_REG_CTRL_OFFSET))
#define TIMER_0_REG_PSCR *((volatile uint32_t *)(TIMER_0_BASE_ADDR + TIMER_0_REG_PSCR_OFFSET))
#define TIMER_0_REG_CNT  *((volatile uint32_t *)(TIMER_0_BASE_ADDR + TIMER_0_REG_CNT_OFFSET))
#define TIMER_0_REG_CMP  *((volatile uint32_t *)(TIMER_0_BASE_ADDR + TIMER_0_REG_CMP_OFFSET))
#define TIMER_0_REG_STAT *((volatile uint32_t *)(TIMER_0_BASE_ADDR + TIMER_0_REG_STAT_OFFSET))

#define SYS_CLK_FREQ 50000000

/**
 * @brief 模拟一个计算密集型任务
 * @param iterations 循环次数
 * @return uint32_t 一个伪结果，以防止编译器优化掉整个循环
 */
uint32_t run_workload_to_test(uint32_t iterations) {
    volatile uint32_t result = 0;
    for (uint32_t i = 0; i < iterations; ++i) {
        result = (result + i) * 3;
    }
    return result;
}

int main(void) {
    printf("Starting performance test...\n");
    const int NUM_TEST_RUNS = 20;
    const int WORKLOAD_ITERATIONS = 1000000;
    uint32_t start_ticks, end_ticks, elapsed_ticks;
    uint32_t total_ticks = 0;
    uint32_t min_ticks = UINT_MAX;
    uint32_t max_ticks = 0;

    printf("Starting performance test...\n");
    printf("Workload: %d iterations of (result + i) * 3\n", WORKLOAD_ITERATIONS);
    printf("--------------------------------------------------\n");

    for (int i = 0; i < NUM_TEST_RUNS; ++i) {
        // --- 开始测量 ---
        start_ticks = 0; // 记录开始时间

        // --- 运行工作负载 ---
        run_workload_to_test(WORKLOAD_ITERATIONS);

        // --- 结束测量 ---
        end_ticks = 0; // 记录结束时间

        elapsed_ticks = end_ticks - start_ticks;

        // --- 打印本次（中间）结果 ---
        printf("[Test Run %d/%d] Workload finished in: %d ticks\n",
               i + 1, NUM_TEST_RUNS, elapsed_ticks);

        // 更新统计数据
        total_ticks += elapsed_ticks;
        if (elapsed_ticks < min_ticks) min_ticks = elapsed_ticks;
        if (elapsed_ticks > max_ticks) max_ticks = elapsed_ticks;
    }

    printf("\n");
    printf("==================================================\n");
    printf("=              Performance Test Report           =\n");
    printf("==================================================\n");
    printf("  Workload Iterations : %d\n", WORKLOAD_ITERATIONS);
    printf("  Number of Test Runs : %d\n", NUM_TEST_RUNS);
    printf("--------------------------------------------------\n");
    printf("  Total Ticks   : %d\n", total_ticks);
    printf("  Min Ticks     : %d (Fastest Run)\n", min_ticks);
    printf("  Max Ticks     : %d (Slowest Run)\n", max_ticks);
    printf("--------------------------------------------------\n");
    printf("  NOTE: Ticks are raw counter values. To get time\n");
    printf("        in seconds, use: time_s = ticks / SYS_CLK_FREQ\n");
    printf("        (SYS_CLK_FREQ = %d Hz)\n", SYS_CLK_FREQ);
    printf("==================================================\n");

    return 0;
}
