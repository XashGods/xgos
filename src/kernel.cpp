#include <stdint.h>
#include "console.h"

// Multiboot loader magic value passed in EAX
static const uint32_t MULTIBOOT_MAGIC = 0x2BADB002;


extern "C" void kernel_main() {
    uint32_t magic;
    // Retrieve magic number from EAX register
    asm volatile("movl %%eax, %0" : "=r"(magic));
    if (magic != MULTIBOOT_MAGIC) {
        kprintf("Invalid MB magic\n");
        for (;;) {}
    }
    kprintf("Hello from XG OS!\n");
    for (;;) {}
}