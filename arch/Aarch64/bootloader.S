/* OrionOS Stage-1 Bootloader (AArch64)
 * Author: Abhigyan Ghosh
 * Licensed under MIT
 * Description: UART bootloader with banner and jump to kernel
 */

.global _start
.extern kernel_entry   // Let linker resolve the real kernel entry

.section .text.boot

UART_BASE   = 0x09000000
UART_DR     = 0x00
UART_FR     = 0x18
FR_TXFF     = (1 << 5)

_start:
    /* Setup temporary stack */
    ldr x0, =boot_stack_top
    mov sp, x0

    /* Print bootloader banner */
    ldr x1, =banner
    bl print_uart_string

    /* Small delay loop */
    mov x0, #50000    // outer loop count
outer_loop:
    mov x1, #40000   // inner loop count
inner_loop:
    subs x1, x1, #1
    b.ne inner_loop
    subs x0, x0, #1
    b.ne outer_loop

    /* Print debug message before kernel jump */
    ldr x1, =jump_msg
    bl print_uart_string

    /* Jump to kernel using symbol */
    ldr x0, =kernel_entry
    br x0

/* --------------------------------------------------- */
/* Subroutine: print a null-terminated string to UART */
print_uart_string:
    ldr x3, =UART_BASE
.next_char:
    ldrb w0, [x1], #1      // load byte, post-increment pointer
    cbz w0, .done_print
.wait_tx:
    ldr w2, [x3, #UART_FR] // load UART flag
    tst w2, #FR_TXFF        // test TX full
    b.ne .wait_tx
    str w0, [x3, #UART_DR] // write char
    b .next_char
.done_print:
    ret

/* --------------------------------------------------- */
/* Boot banner */
.section .rodata
banner:
    .ascii "\033[1;36m"
    .ascii "=====================================\n"
    .ascii "        ORIONOS\n"
    .ascii " OrionOS is initializing\n"
    .ascii "=====================================\n"
    .ascii "\033[0m\n"
    .byte 0

jump_msg:
    .ascii "\nOrionOS is initializing\n"
    .byte 0

/* --------------------------------------------------- */
/* Temporary stack */
.section .bss
.align 16
boot_stack:
    .skip 4096
boot_stack_top:
