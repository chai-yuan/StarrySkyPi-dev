.section .text.start
.globl _start
.type _start, @function

_start:
  la   a0, _bss_start
  la   a1, _bss_end
bss_loop:
  beq  a0, a1, bss_done
  sw   zero, 0(a0)
  addi a0, a0, 4
  j    bss_loop
bss_done:

  la sp, _stack_top
  call main

hang:
  j hang
