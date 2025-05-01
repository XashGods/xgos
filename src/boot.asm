; Multiboot header and entry stub for XG OS
; Assembled with NASM (ELF32)
bits 32

section .text
    align 4
    dd 0x1BADB002                   ; Multiboot header magic
    dd 0x00000003                   ; Flags: align modules, memory information
    dd -(0x1BADB002 + 0x00000003)   ; Checksum

    global _start
    extern kernel_main
    extern init_global_ctors

_start:
    ; Preserve multiboot magic (in EAX) while calling constructors
    push eax
    call init_global_ctors
    pop eax
    call kernel_main      ; Jump to C++ kernel entry point