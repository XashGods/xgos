#include <stdint.h>

// Multiboot loader magic value passed in EAX
static const uint32_t MULTIBOOT_MAGIC = 0x2BADB002;

// Simple VGA text mode print (gray on black)
static void print(const char* str) {
    volatile uint16_t* video = (volatile uint16_t*)0xB8000;
    for (uint32_t i = 0; str[i] != '\0'; ++i) {
        video[i] = (uint16_t)str[i] | (uint16_t)(0x07 << 8);
    }
}

extern "C" void kernel_main() {
    uint32_t magic;
    // Retrieve magic number from EAX register
    asm volatile("movl %%eax, %0" : "=r"(magic));
    if (magic != MULTIBOOT_MAGIC) {
        print("Invalid MB magic");
        for (;;) {}
    }
    print("Hello from XG OS!");
    for (;;) {}
}