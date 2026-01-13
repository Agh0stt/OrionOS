/* Project Orion - OrionOS - Abhigyan Ghosh

Lisenced Under the MIT lisence

Copyright - 2026 - present Abhigyan Ghosh

Project OrionOS - Aarch64

start.S - bootloader
*/
.global kernel_entry
.section .text.boot

kernel_entry:
    ldr x30, =stack_top
    mov sp, x30
    bl kmain
    b .
