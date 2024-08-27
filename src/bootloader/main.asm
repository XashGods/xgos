.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .text
.global _start
.type _start, @function
_start:
    # Initialize stack pointer
    mov $stack_top, %esp

    # Call the kernel main function
    call kernel_main

    # Halt the CPU
    cli
1:  hlt
    jmp 1b

.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:
