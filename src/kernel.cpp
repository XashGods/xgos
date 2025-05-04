#include <stdint.h>
#include "console.h"
#include "memory.h"
#include "paging.h"

// Multiboot loader magic value passed in EAX
static const uint32_t MULTIBOOT_MAGIC = 0x2BADB002;


extern "C" void kernel_main() {
    uint32_t magic, mbi;
    // Retrieve magic number and multiboot info address
    asm volatile("movl %%eax, %0" : "=r"(magic));
    asm volatile("movl %%ebx, %0" : "=r"(mbi));
    if (magic != MULTIBOOT_MAGIC) {
        kprintf("Invalid MB magic\n");
        for (;;) {}
    }
    // memory management
    memory_init(mbi);
    // identity map all memory and enable paging
    paging_init();
    kprintf("Hello from XG OS!\n");
    for (;;) {}
}