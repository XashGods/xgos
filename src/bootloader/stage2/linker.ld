OUTPUT_FORMAT(binary)
ENTRY(_x86_Video_WriteCharTeletype)  /* Entry point is the _x86_Video_WriteCharTeletype function */
OUTPUT_ARCH(i386)

SECTIONS
{
    . = 0x1000;  /* Set the starting address to 0x1000 to leave space for Multiboot header */
    
    /* Multiboot header section */
    .multiboot : {
        KEEP(*(.multiboot))  /* Keep the Multiboot header */
    }
    
    /* Code segment */
    .text : {
        *(_TEXT)   /* All code in the _TEXT section */
        *(.text)   /* Additional text sections if any */
    }

    /* Read-Only Data segment */
    .rodata : {
        *(.rodata)
    }
    
    /* Data segment (initialized data) */
    .data : {
        *(.data)
    }
    
    /* Uninitialized data (BSS) */
    .bss : {
        *(COMMON)
        *(.bss)
    }
    
    /* Stack setup (if needed) */
    .stack (NOLOAD) : {
        __stack_top = . + 0x200;  /* Set up the stack with 0x200 size */
    }
    
    /DISCARD/ : {
        *(.note.gnu.property)   /* Discard unwanted sections */
        *(.comment)
        *(.note*)
    }
}