#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stddef.h>

// multiboot information structure (partial)
typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
} multiboot_info_t;

// memory map entry
typedef struct __attribute__((packed)) {
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} memory_map_entry_t;

// memory manager/physical frame allocator
// mbi_addr: address of multiboot info structure
void memory_init(uint32_t mbi_addr);

// debug: dump the raw memory map as provided by Multiboot
void memory_dump_map(uint32_t mbi_addr);

// allocate a 4 KiB physical frame; returns physical address or 0 on failure
uint64_t frame_alloc();

// free a previously allocated 4 KiB physical frame
void frame_free(uint64_t paddr);

// simple kernel heap allocator (bump allocator)
// size: number of bytes to allocate
void *kmalloc(size_t size);

// return number of physical frames detected
uint64_t memory_get_nframes(void);

#endif // MEMORY_H
