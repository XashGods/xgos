OUTPUT_FORMAT("elf32-i386")
ENTRY(_start)
SECTIONS
{
    . = 1M;
    .text : { *(.text*) }
    .init_array : {
        __init_array_start = .;
        KEEP(*(.init_array))
        KEEP(*(.init_array.*))
        __init_array_end = .;
    }
    .rodata : { *(.rodata*) }
    .data : { *(.data*) }
    .bss : {
        __bss_start = .;
        *(.bss*)
        __bss_end = .;
    }
}