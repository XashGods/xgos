#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stddef.h>

// multiboot information structure (extended for framebuffer)
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
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;

    union {
        struct {
            uint32_t framebuffer_palette_addr;
            uint16_t framebuffer_palette_num_colors;
        };

        struct {
            uint8_t framebuffer_red_field_position;
            uint8_t framebuffer_red_mask_size;
            uint8_t framebuffer_green_field_position;
            uint8_t framebuffer_green_mask_size;
            uint8_t framebuffer_blue_field_position;
            uint8_t framebuffer_blue_mask_size;
        };
    };
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

// framebuffer detection and setup
// returns 1 if framebuffer is available, 0 otherwise
int framebuffer_detect(uint32_t mbi_addr);

// get framebuffer information from multiboot
void framebuffer_get_info(uint32_t mbi_addr, uint64_t *addr, uint32_t *width,
                          uint32_t *height, uint32_t *pitch, uint8_t *bpp);

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
