#include <stdio.h>
#include <stdint.h>
#include <string.h>

extern char _binary_app_bin_start[];
extern char _binary_app_bin_end[];
extern char _binary_app_bin_size[];

#define TARGET_ADDR 0x0f000000

int main() {
    printf("===== Bootloader Started =====\n");

    const char* source_addr = _binary_app_bin_start;
    uint32_t app_size = _binary_app_bin_end - _binary_app_bin_start;

    char* dest_addr = (char*)TARGET_ADDR;

    printf("Bootloader is preparing to load the application.\n");
    printf("  - Application source address (in bootloader's memory): %p\n", source_addr);
    printf("  - Application destination address (target RAM):        0x%x\n", TARGET_ADDR);
    printf("  - Application size: %d bytes\n\n", app_size);

    if (app_size == 0) {
        printf("[ERROR] Embedded application size is zero. Halting.\n");
        while(1);
    }

    printf("Step 1: Copying %d bytes from %p to %p...\n", app_size, source_addr, dest_addr);
    memcpy(dest_addr, source_addr, app_size);
    printf("Step 1: Copy complete.\n\n");

    printf("Step 2: Verifying the integrity of the copy...\n");
    // memcmp 比较两块内存区域的内容
    // 如果内容完全相同，它返回 0
    int verification_result = memcmp(source_addr, dest_addr, app_size);
    if (verification_result == 0) {
        printf("Step 2: Verification successful! Application is ready to run.\n\n");
    } else {
        printf("[FATAL ERROR] Memory verification failed! The copy is corrupt. Halting.\n");
        while(1);
    }

    printf("Step 3: Preparing to jump to the application at 0x%x...\n", TARGET_ADDR);
    printf("=========================================================\n");

    typedef void (*app_entry_t)(void);

    app_entry_t app_entry = (app_entry_t)TARGET_ADDR;

    app_entry();

    printf("[ERROR] Jump failed! This should never happen.\n");
    while(1);

    return 0;
}
