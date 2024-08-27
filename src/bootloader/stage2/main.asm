bits 16

section .multiboot
    align 4
    dd 0x1BADB002              ; magic number
    dd 0x00                    ; flags
    dd - (0x1BADB002 + 0x00)   ; checksum

section .text

global _start
extern cstart_

_start:
    cli
    xor ax, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    mov ax, 0
    mov ds, ax
    mov es, ax

    xor dh, dh
    push dx
    call cstart_

    cli
    hlt

times 510-($-$$) db 0
dw 0xAA55