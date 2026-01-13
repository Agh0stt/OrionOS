ENTRY(_start)

SECTIONS {
    /* Bootloader */
    . = 0x40080000;
    .text.boot : {
        bootloader.o(.text.boot)
    }

    /* Kernel */
    . = 0x40100000;
    .text : {
        kernel.o(.text)
        start.o(.text.boot)
    }

    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss : { *(.bss) *(COMMON) }

    . = ALIGN(8);
    . += 0x2000;
    stack_top = .;
}
