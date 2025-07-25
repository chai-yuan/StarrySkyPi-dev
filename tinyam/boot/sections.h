#ifndef __SECTIONS_H__
#define __SECTIONS_H__

// 宏定义，用于标记 bootloader 的代码段
#define BOOT_TEXT __attribute__((section(".boot.text")))

// 宏定义，用于标记 bootloader 的只读数据段
#define BOOT_RODATA __attribute__((section(".boot.rodata")))

#endif // __SECTIONS_H__
