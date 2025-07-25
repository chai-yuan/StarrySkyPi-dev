OUTPUT_ARCH( "riscv" )
ENTRY( _entry )

MEMORY
{
  /* 应用程序运行的 RAM 区域 */
  RAM   (rwx) : ORIGIN = 0x0f000000, LENGTH = 1M
  /* Bootloader 和应用程序镜像存放的 FLASH 区域 */
  FLASH (rx)  : ORIGIN = 0x30000000, LENGTH = 16M
}

SECTIONS
{
  /* VMA 和 LMA 都在 FLASH */
  .bootloader 0x30000000 :
  {
    /* 确保启动代码放在最前面 */
    *(.text.entry)
    *(.boot.text .boot.text.*)
    *(.boot.rodata .boot.rodata.*)
    . = ALIGN(4);
    PROVIDE(_bootloader_end = .);
  } > FLASH AT > FLASH

  /* 应用程序代码段 */
  /* VMA 在 RAM, LMA 在 FLASH, 紧跟在 bootloader 后面 */
  .text :
  {
    /* 提供加载地址的起始符号，供 bootloader 使用 */
    PROVIDE(_sidata = LOADADDR(.text));

    /* 应用程序的代码段 */
    /* 包含所有非 boot 目录下的代码 */
    *(.text .text.*)
    *(.rodata .rodata.*)
    . = ALIGN(0x1000);
  } > RAM AT > FLASH

  . = ALIGN(0x1000);
  PROVIDE(etext = .);

  /* 应用程序的已初始化数据段 */
  /* VMA 在 RAM, LMA 在 FLASH, 紧跟在 .text 的 LMA 后面 */
  .data : {
    /* 提供 VMA 的起始符号 */
    PROVIDE(_sdata = .);
    *(.data .data.*)
    PROVIDE(_edata = .);
  } > RAM AT > FLASH

  /* 应用程序的未初始化数据段 (BSS) */
  /* 只有 VMA，没有 LMA，因为不需要从 Flash 加载 */
  .bss (NOLOAD) : {
    PROVIDE(_sbss = .);
    *(.bss .bss.*)
    *(.sbss .sbss.*)
    *(COMMON)
    PROVIDE(_ebss = .);
    PROVIDE(end = .); /* 堆的起始地址 */
  } > RAM
}
