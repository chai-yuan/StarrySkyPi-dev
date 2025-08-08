.section .text
.globl _start
.section .text.entry
.globl _entry
_entry:
    call _start
junk:
    j junk
