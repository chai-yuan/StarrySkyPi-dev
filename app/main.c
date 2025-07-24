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
 * @brief 初始化定时器作为高性能计数器
 *        - 预分频器设置为1，以获得最高分辨率
 *        - 比较值设置为最大，以防止计数器中途复位
 */
void performance_counter_init() {
    printf("Performance Counter INIT:\n");
    TIMER_0_REG_CTRL = 0x0; // 禁用定时器以便配置
    TIMER_0_REG_PSCR = 0;   // 预分频值 (div-1)，0表示1分频，即不分频
    TIMER_0_REG_CMP = 0xFFFFFFFF; // 设置为最大值，使其永远不会达到比较匹配
    printf("  PSCR: %d, CMP: 0x%d\n", TIMER_0_REG_PSCR, TIMER_0_REG_CMP);
    printf("Performance Counter INIT DONE\n\n");
}

/**
 * @brief 重置计数器为0并开始计数
 *        - 控制值 0xD (二进制 1101) 可能是: 使能定时器 + 启动计数 + 自动重载(如果支持)
 */
void performance_counter_reset_and_start() {
    // 停止定时器并确保计数器清零(某些硬件需要先停止才能清零)
    TIMER_0_REG_CTRL = 0x0;
    // (可选) 某些硬件通过写0来重置计数器，而有些则在启动时自动重置。
    // 这里我们假设启动时会自动从0开始。
    TIMER_0_REG_CTRL = 0xD; // 启动定时器
}

/**
 * @brief 获取当前计数器的计数值 (ticks)
 * @return uint32_t 当前的计数值
 */
uint32_t performance_counter_get_ticks() {
    return TIMER_0_REG_CNT;
}

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


int main() {
    const int NUM_TEST_RUNS = 20;
    const int WORKLOAD_ITERATIONS = 1000000;
    uint32_t start_ticks, end_ticks, elapsed_ticks;
    uint32_t total_ticks = 0;
    uint32_t min_ticks = UINT_MAX;
    uint32_t max_ticks = 0;

    performance_counter_init();

    printf("Starting performance test...\n");
    printf("Workload: %d iterations of (result + i) * 3\n", WORKLOAD_ITERATIONS);
    printf("--------------------------------------------------\n");

    for (int i = 0; i < NUM_TEST_RUNS; ++i) {
        // --- 开始测量 ---
        performance_counter_reset_and_start();
        start_ticks = performance_counter_get_ticks(); // 记录开始时间

        // --- 运行工作负载 ---
        run_workload_to_test(WORKLOAD_ITERATIONS);

        // --- 结束测量 ---
        end_ticks = performance_counter_get_ticks(); // 记录结束时间

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
