.section .text.start
.globl _start
.type _start, @function

_start:
  la sp, 0x0f010000
  jal _loader
  la sp, 0x0f010000
  mv s0, zero
  call main

  /* Should never reach here. Infinite loop for safety. */
hang:
  j hang


_loader:
  /*
   * Copy application's .text section from FLASH (LMA) to RAM (VMA)
   */
  la a0, _stext_load    /* a0 = Source address in FLASH */
  la a1, _stext         /* a1 = Destination address in RAM */
  la a2, etext          /* a2 = End of destination address in RAM */
copy_text_loop:
  beq a1, a2, copy_text_done /* If destination == end, we are done */
  lw a3, 0(a0)               /* Load a word from FLASH */
  sw a3, 0(a1)               /* Store a word to RAM */
  addi a0, a0, 4             /* Move to next word in source */
  addi a1, a1, 4             /* Move to next word in destination */
  j copy_text_loop
copy_text_done:

  /*
   * Copy application's .data section from FLASH (LMA) to RAM (VMA)
   */
  la a0, _sdata_load /* a0 = Source address in FLASH */
  la a1, _sdata      /* a1 = Destination address in RAM */
  la a2, _edata      /* a2 = End of destination address in RAM */
copy_data_loop:
  beq a1, a2, copy_data_done /* If destination == end, we are done */
  lw a3, 0(a0)               /* Load a word from FLASH */
  sw a3, 0(a1)               /* Store a word to RAM */
  addi a0, a0, 4             /* Move to next word in source */
  addi a1, a1, 4             /* Move to next word in destination */
  j copy_data_loop
copy_data_done:

  /*
   * Clear the .bss section in RAM.
   */
  la a0, _sbss /* a0 = Start of .bss */
  la a1, _ebss /* a1 = End of .bss */
clear_bss_loop:
  beq a0, a1, clear_bss_done /* If current address == end, we are done */
  sw zero, 0(a0)             /* Store zero to the current word */
  addi a0, a0, 4             /* Move to the next word */
  j clear_bss_loop
clear_bss_done:
  ret
