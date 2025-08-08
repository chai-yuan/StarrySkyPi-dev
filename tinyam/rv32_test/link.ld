OUTPUT_ARCH( "riscv" )
ENTRY( _entry )

MEMORY
{
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 64M
}

SECTIONS
{
  . = ORIGIN(RAM);

  .text :
  {
    *(.text.entry)
    *(.text .text.*)
  } > RAM

  .rodata :
  {
    . = ALIGN(8);
    *(.rodata .rodata.*)
  } > RAM

  .data :
  {
    . = ALIGN(8);
    *(.data .data.*)
  } > RAM

  .bss :
  {
    . = ALIGN(8);
    _bss_start = .;
    *(.bss .bss.*)
    *(COMMON)
    _bss_end = .;
  } > RAM

  _end = _bss_end;

  _heap_start = _end;
  _heap_end = ORIGIN(RAM) + LENGTH(RAM); /* 堆的理论结束地址 (RAM 的顶部) */
  _stack_top = ORIGIN(RAM) + LENGTH(RAM); /* 栈顶指针的初始值 */
}
