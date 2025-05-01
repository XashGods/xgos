/* Minimal linker script for XG OS */
OUTPUT_FORMAT("elf32-i386")
ENTRY(_start)
SECTIONS
{
    . = 1M;
    .text : { *(.text*) }
    .rodata : { *(.rodata*) }
    .data : { *(.data*) }
    .bss : {
        __bss_start = .;
        *(.bss*)
        __bss_end = .;
    }
}