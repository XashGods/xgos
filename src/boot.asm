; Multiboot header and entry stub for XG OS
; Assembled with NASM (ELF64)
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
    ; switch to long mode
    mov esp, stack_top
    
    ; save multiboot info
    mov [multiboot_magic], eax
    mov [multiboot_info], ebx
    
    ; check for CPUID support
    call check_cpuid
    call check_long_mode
    
    ; set up paging for long mode
    call setup_page_tables
    call enable_paging
    
    ; load GDT
    lgdt [gdt64.pointer]
    
    ; jump to long mode
    jmp gdt64.code:long_mode_start

check_cpuid:
    ; check if CPUID is supported by attempting to flip the ID bit (bit 21) in EFLAGS
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 1 << 21
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    xor eax, ecx
    jz .no_cpuid
    ret
.no_cpuid:
    mov al, "1"
    jmp error

check_long_mode:
    ; check for extended processor info
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .no_long_mode
    
    ; check for long mode
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz .no_long_mode
    ret
.no_long_mode:
    mov al, "2"
    jmp error

setup_page_tables:
    ; map first P4 entry to P3 table
    mov eax, p3_table
    or eax, 0b11 ; present + writable
    mov [p4_table], eax
    
    ; clear upper 32 bits of P4 entry
    mov dword [p4_table + 4], 0
    
    ; map first P3 entry to P2 table
    mov eax, p2_table
    or eax, 0b11 ; present + writable
    mov [p3_table], eax
    
    ; clear upper 32 bits of P3 entry
    mov dword [p3_table + 4], 0
    
    ; map each P2 entry to a huge 2MiB page
    mov ecx, 0
.map_p2_table:
    mov eax, 0x200000  ; 2MiB
    mul ecx            ; start address of ecx-th page
    or eax, 0b10000011 ; present + writable + huge
    mov [p2_table + ecx * 8], eax
    
    ; clear upper 32 bits of each P2 entry
    mov dword [p2_table + ecx * 8 + 4], 0
    
    inc ecx
    cmp ecx, 512       ; if counter == 512, the whole P2 table is mapped
    jne .map_p2_table  ; else map the next entry
    ret

enable_paging:
    ; load P4 to cr3 register (cpu uses this to access the P4 table)
    mov eax, p4_table
    mov cr3, eax
    
    ; enable PAE-flag in cr4 (Physical Address Extension)
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax
    
    ; set the long mode bit in the EFER MSR (model specific register)
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr
    
    ; enable paging in the cr0 register
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax
    ret

bits 64
long_mode_start:
    ; load 64-bit data segment
    mov ax, gdt64.data
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; set up stack pointer for 64-bit mode
    mov rsp, stack_top
    
    ; load multiboot parameters for kernel_main
    mov rdi, [multiboot_magic]  ; first argument (magic)
    mov rsi, [multiboot_info]   ; second argument (mbi_addr)
    
    ; call global constructors
    call init_global_ctors
    
    ; jump to kernel main with parameters in rdi, rsi (System V ABI)
    call kernel_main
    
    ; hang if kernel returns
    hlt

error:
    ; print "ERR: X" where X is the error code
    mov dword [0xb8000], 0x4f524f45
    mov dword [0xb8004], 0x4f3a4f52
    mov dword [0xb8008], 0x4f204f20
    mov byte  [0xb800a], al
    hlt

section .bss
    align 4096
p4_table:
    resb 4096
p3_table:
    resb 4096
p2_table:
    resb 4096
stack_bottom:
    resb 4096 * 4
stack_top:

section .data
    align 8
multiboot_magic:
    dd 0
multiboot_info:
    dd 0

section .rodata
gdt64:
    dq 0 ; zero entry
.code: equ $ - gdt64
    dq (1<<44) | (1<<47) | (1<<41) | (1<<43) | (1<<53) ; code segment
.data: equ $ - gdt64
    dq (1<<44) | (1<<47) | (1<<41) ; data segment
.pointer:
    dw $ - gdt64 - 1
    dq gdt64