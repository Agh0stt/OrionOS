/* Project Orion - OrionOS - Abhigyan Ghosh
* Lisenced Under the MIT lisence
* Copyright - 2026 - present Abhigyan Ghosh
* Project OrionOS - Aarch64
* linker.ld - linker Script
*/
ENTRY(_start)

SECTIONS {
    . = 0x40100000;
    .text.boot : { start.o(.text.boot) }
    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss) *(COMMON) }
    . = ALIGN(8);
    . += 0x1000;  /* 4KB stack */
    stack_top = .;
}
