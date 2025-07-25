.section .text
.globl start
.section .text.entry
.globl _entry
_entry:
UART_INIT:
    lui a4,0x10000
    lbu a5,3(a4)
    ori a5,a5,-128
    andi a5,a5,255
    sb a5,3(a4)
    li a5,13
    sb a5,0(a4)
    li a5,3
    sb a5,3(a4)

    # 跳转到start
    la sp, 0x0f010000
    call start
junk:
    j junk
