#include <stdint.h>
#include "console.h"
#include "memory.h"
#include "paging.h"

// early debug function to write directly to VGA memory
static void early_print(const char *msg) {
    static int pos = 0;
    volatile uint16_t *vga = (volatile uint16_t *) 0xB8000;
    while (*msg) {
        vga[pos++] = 0x0F00 | *msg; // white on black
        msg++;
    }
}

// Multiboot loader magic value passed in RAX
static const uint32_t MULTIBOOT_MAGIC = 0x2BADB002;

extern "C" void kernel_main(uint64_t magic, uint64_t mbi_addr) {
    early_print("64BIT START");

    if ((uint32_t) magic != MULTIBOOT_MAGIC) {
        early_print("BAD MAGIC");
        for (;;) {
        }
    }

    early_print("MAGIC OK");

    // memory management
    memory_init((uint32_t) mbi_addr);

    early_print("MEM OK");

    // set up advanced paging
    paging_init();

    early_print("PAGE OK");

    clear();
    set_color(VGA_COLOR_RED);
    kprintf("Hello from XG OS 64-bit!\n");
    kprintf("Running in long mode\n");
    for (;;) {
    }
}
