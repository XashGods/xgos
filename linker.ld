OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH("i386:x86-64")
ENTRY(_start)

SECTIONS
{
    . = 1M;
    
    .text ALIGN(4K) : {
        *(.text*)i
    }
    
    .rodata ALIGN(4K) : {
        *(.rodata*)
    }
    
    .init_array ALIGN(4K) : {
        __init_array_start = .;
        KEEP(*(.init_array))
        KEEP(*(.init_array.*))
        __init_array_end = .;
    }
    
    .data ALIGN(4K) : {
        *(.data*)
    }
    
    .bss ALIGN(4K) : {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    }
    
    /* ensure we don't have any unexpected sections */
    /DISCARD/ : {
        *(.eh_frame)
        *(.comment)
        *(.note.gnu.build-id)
    }
}