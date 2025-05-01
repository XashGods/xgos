# XG OS Development Roadmap

## Table of Contents

1. [Toolchain & Prerequisites](#1-toolchain--prerequisites)
2. [Bootloader & Multiboot](#2-bootloader--multiboot)
3. [Kernel Initialization & C++ Runtime](#3-kernel-initialization--c-runtime)
4. [System Fundamentals: GDT, IDT & Interrupts](#4-system-fundamentals-gdt-idt--interrupts)
5. [Memory Management](#5-memory-management)
6. [Device Drivers: VGA/Framebuffer & Keyboard](#6-device-drivers-vgaframebuffer--keyboard)
7. [Simple File System Support](#7-simple-file-system-support)
8. [User-Space Shell & Command Interpreter](#8-user-space-shell--command-interpreter)
9. [Graphics Infrastructure & UI Primitives](#9-graphics-infrastructure--ui-primitives)
10. [Basic Window Manager & UI Components](#10-basic-window-manager--ui-components)
11. [Multitasking (Optional)](#11-multitasking)
12. [ISO Creation & Distribution](#12-iso-creation--distribution)
13. [Further Enhancements & Documentation](#13-further-enhancements--documentation)

---

## 1. Toolchain & Prerequisites

- **Cross-Compiler**  
  – Configure `i686-elf-gcc`/`g++`, `binutils`  
  – Build freestanding toolchain with flags: `-ffreestanding`, `-fno-exceptions`, `-fno-rtti`
- **Assembler & Linker**  
  – NASM or YASM for 16-bit/32-bit assembly  
  – GNU ld or LLD with a custom linker script (`.ld`)
- **Emulation & Debugging**  
  – QEMU, Bochs for rapid iteration  
  – Serial console / GDB stub for low-level debugging
- **Version Control & Documentation**  
  – Git repository (GitHub/GitLab)  
  – OSDev Wiki, “Jump into OSDev”, Bran’s Kernel Tutorial

---

## 2. Bootloader & Multiboot

1. **Multiboot Header**
    - Embed Multiboot 1/2 header for GRUB compatibility
2. **Real-Mode → Protected-Mode**
    - Initialize segment registers, stack
    - Enable A20 line, switch to 32-bit protected mode
3. **Stage 1/Stage 2 Loader (optional)**
    - Custom loader to parse disk structures and load the kernel
4. **Integration & Testing**
    - `grub-mkrescue` + QEMU for ISO boot tests
    - Validate Multiboot info structure in C++ kernel entry point

---

## 3. Kernel Initialization & C++ Runtime

- **Linker Script Layout**
    - Define sections: `.text`, `.rodata`, `.data`, `.bss`
- **C++ Constructors & Runtime**
    - Invoke global/static constructors
    - Disable exceptions/RTTI or implement minimal handlers
- **Debug Output**
    - `kprintf()` to VGA text mode or serial port (COM1)

---

## 4. System Fundamentals: GDT, IDT & Interrupts

- **Global Descriptor Table (GDT)**
    - Code/data segments, TSS entry
- **Interrupt Descriptor Table (IDT)**
    - Define ISRs for exceptions (0–31)
    - IRQ remapping via PIC → APIC (optional)
- **Hardware IRQs**
    - Initialize PIT (timer) for system ticks
    - PS/2 keyboard IRQ handler

---

## 5. Memory Management

1. **Physical Memory Manager**
    - Bitmap or free-list allocator for 4 KiB frames
2. **Paging & Virtual Memory**
    - Set up page directory/table for identity mapping
    - Implement dynamic page allocation
3. **Kernel Heap**
    - Simple bump allocator → slab allocator for objects

---

## 6. Device Drivers: VGA/Framebuffer & Keyboard

- **VGA Text Mode**
    - Cursor control, color attributes, optimized writes
- **VESA/VBE Framebuffer**
    - Switch to a linear 32-bit color mode
    - Basic pixel drawing API
- **PS/2 Keyboard Driver**
    - Scancode set decoding → ASCII/scan codes

---

## 7. Simple File System Support

1. **Block I/O Layer**
    - IDE/ATA driver for master/slave PATA
2. **FAT12/16 Implementation**
    - Parse Boot Sector, FAT tables, directory entries
    - Read files from ISO9660/FAT-formatted image
3. **Testing**
    - Mount ISO with a FAT12 floppy image
    - Read simple text files, display on screen

---

## 8. User-Space Shell & Command Interpreter

- **Command Parser**
    - Tokenization, argument handling
- **Built-in Commands**
    - `help`, `ls`, `cat`, `cd`
- **External Executables**
    - Implement a simple ELF loader (optional)

---

## 9. Graphics Infrastructure & UI Primitives

- **Drawing Primitives**
    - `putPixel`, `drawLine`, `drawRect`, `fillCircle`, etc...
- **Bitmap Font Renderer**
    - Fixed-width font, glyph atlas in ROM/data
- **Input Event System**
    - PS/2 mouse driver, cursor sprite
    - Central event queue with keyboard/mouse events

---

## 10. Basic Window Manager & UI Components

1. **Window Abstraction**
    - Position, size, z-order, clipping
    - Title bar, border drawing
2. **UI Widgets**
    - Buttons, checkboxes, textboxes, menus, icons
    - Event dispatching (focus, mouse capture)
3. **Rendering Optimization**
    - Dirty-region tracking, double-buffering

---

## 11. Multitasking

- **Task Control Block (TCB)**
    - Register state, stack pointer, priority
- **Context Switching**
    - Save/restore CPU state in software interrupt or timer ISR
- **Synchronization Primitives**
    - Spinlocks, mutexes, semaphores

---

## 12. ISO Creation & Distribution

- **Bootable ISO Generation**
  ```bash
  grub-mkrescue -o myos.iso iso_root/
  ```
- **ISO9660 + El Torito**
  - `mkisofs`/`genisoimage` options for CD boot
- **Automated Testing**
  - Boot scripts in QEMU/Bochs, memory and graphics smoke tests

---

## 13. Further Enhancements & Documentation
- **Advanced File Systems** 
  - FAT32, EXT2/3, custom FS
- **ELF Loader & Dynamic Linker**
- **Network Stack**
  - e1000/RTL8139 driver, TCP/IP, basic sockets
- **C++ Core Library**
  - Containers, algorithms, string utilities
- **Project Documentation**
  - Build instructions, architecture overview, API reference
  - Tutorial-style “Getting Started” guide