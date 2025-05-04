#include "memory.h"
#include "console.h"
#include <stdint.h>
#include <stddef.h>

extern "C" uint32_t __bss_end;

static uint8_t* frame_bitmap = nullptr;
static uint32_t nframes = 0;
static uint32_t bitmap_size = 0; // bytes

// frame bitmap helpers (mark/test frames)
static inline void frame_set(uint32_t idx) { frame_bitmap[idx / 8] |= (uint8_t)(1 << (idx % 8)); }
static inline void frame_clear(uint32_t idx) { frame_bitmap[idx / 8] &= (uint8_t)~(1 << (idx % 8)); }
static inline bool frame_test(uint32_t idx) { return frame_bitmap[idx / 8] & (uint8_t)(1 << (idx % 8)); }

// dump raw memory map entries for debugging (E820)
void memory_dump_map(uint32_t mbi_addr) {
    multiboot_info_t* mbi = (multiboot_info_t*)(uintptr_t)mbi_addr;
    if (!(mbi->flags & (1 << 6))) return;
    uint8_t* cur = (uint8_t*)(uintptr_t)mbi->mmap_addr;
    uint8_t* end = cur + mbi->mmap_length;
    kprintf("[Memory Map] addr=0x%x len=%u\n", mbi->mmap_addr, mbi->mmap_length);
    uint32_t idx = 0;
    while (cur < end) {
        // struct: [uint32_t size][uint64_t base][uint64_t len][uint32_t type]
        uint32_t sz = *(uint32_t*)(uintptr_t)cur;
        uint64_t base = *(uint64_t*)(uintptr_t)(cur + 4);
        uint64_t len  = *(uint64_t*)(uintptr_t)(cur + 12);
        uint32_t type = *(uint32_t*)(uintptr_t)(cur + 20);
        kprintf("  [%u] base=0x%x len=0x%x type=%u\n", idx++, (uint32_t)base, (uint32_t)len, type);
        cur += sz + 4;
    }
}

void memory_init(uint32_t mbi_addr) {
    multiboot_info_t* mbi = (multiboot_info_t*)(uintptr_t)mbi_addr;
    bool has_map = mbi->flags & (1 << 6);
    // print memory-info fields
    kprintf("mem_lower=%uKB mem_upper=%uKB\n", mbi->mem_lower, mbi->mem_upper);
    if (has_map) {
        kprintf("mmap_addr=0x%x mmap_length=%u\n", mbi->mmap_addr, mbi->mmap_length);
        memory_dump_map(mbi_addr);
    } else {
        kprintf("memory_init: no memory map, falling back to mem_upper\n");
    }

    // highest available address
    uint64_t max_addr = 0;
    if (has_map) {
        uint8_t* cur = (uint8_t*)(uintptr_t)mbi->mmap_addr;
        uint8_t* end = cur + mbi->mmap_length;
        while (cur < end) {
            uint32_t sz   = *(uint32_t*)(uintptr_t)cur;
            uint64_t base = *(uint64_t*)(uintptr_t)(cur + 4);
            uint64_t len  = *(uint64_t*)(uintptr_t)(cur + 12);
            uint32_t type = *(uint32_t*)(uintptr_t)(cur + 20);
            if (type == 1) {
                uint64_t top = base + len;
                if (top > max_addr) max_addr = top;
            }
            cur += sz + 4;
        }
    } else {
        // mem_upper is KB above 1 MiB
        max_addr = (uint64_t)mbi->mem_upper * 1024 + 0x100000;
    }
    // total frames available
    nframes = (uint32_t)(max_addr / 4096);
    bitmap_size = (nframes + 7) / 8;

    // place frame bitmap after end of BSS
    frame_bitmap = (uint8_t*)(uintptr_t)&__bss_end;
    // mark all frames as used
    for (uint32_t i = 0; i < bitmap_size; ++i) frame_bitmap[i] = 0xFF;
    // reserve frames occupied by the bitmap itself
    {
        uint32_t start = ((uintptr_t)frame_bitmap) / 4096;
        uint32_t end = (((uintptr_t)frame_bitmap + bitmap_size) + 4095) / 4096;
        for (uint32_t f = start; f < end; ++f) frame_set(f);
    }
    // free frames using the map or fallback
    if (has_map) {
        uint8_t* cur = (uint8_t*)(uintptr_t)mbi->mmap_addr;
        uint8_t* end = cur + mbi->mmap_length;
        while (cur < end) {
            uint32_t sz   = *(uint32_t*)(uintptr_t)cur;
            uint64_t base = *(uint64_t*)(uintptr_t)(cur + 4);
            uint64_t len  = *(uint64_t*)(uintptr_t)(cur + 12);
            uint32_t type = *(uint32_t*)(uintptr_t)(cur + 20);
            if (type == 1) {
                uint64_t addr_end = base + len;
                for (uint64_t p = base; p + 4096 <= addr_end; p += 4096) {
                    frame_clear((uint32_t)(p / 4096));
                }
            }
            cur += sz + 4;
        }
    } else {
        // free frames above 1 MiB
        uint32_t first = 0x100000 / 4096;
        for (uint32_t i = first; i < nframes; ++i) frame_clear(i);
    }

    // reserve frames for kernel (up to end of BSS)
    uint32_t bss_end = (uint32_t)(uintptr_t)&__bss_end;
    uint32_t kernel_end = (bss_end + 4095) & ~4095u;
    for (uint32_t p = 0; p < kernel_end; p += 4096) frame_set(p / 4096);
    // reserve frames for loaded modules, if any
    if (mbi->flags & (1 << 3)) {
        typedef struct { uint32_t mod_start, mod_end, string, reserved; } module_t;
        module_t* mods = (module_t*)(uintptr_t)mbi->mods_addr;
        kprintf("Modules (%u):\n", mbi->mods_count);
        for (uint32_t i = 0; i < mbi->mods_count; ++i) {
            uint32_t ms = mods[i].mod_start;
            uint32_t me = mods[i].mod_end;
            const char* name = (const char*)(uintptr_t)mods[i].string;
            kprintf("  [%u] 0x%x-0x%x '%s'\n", i, ms, me, name);
            // reserve the frames spanned by this module
            uint32_t start = ms & ~4095u;
            uint32_t end = (me + 4095) & ~4095u;
            for (uint32_t p = start; p < end; p += 4096) frame_set(p / 4096);
        }
    }

    // count free frames
    uint32_t free_frames = 0;
    for (uint32_t i = 0; i < nframes; ++i) {
        uint32_t byte = i / 8;
        uint8_t bit = (uint8_t)(1 << (i % 8));
        if (!(frame_bitmap[byte] & bit)) {
            free_frames++;
        }
    }
    kprintf("frames total=%u free=%u reserved=%u\n", nframes, free_frames, nframes - free_frames);
}

// allocate a 4 KiB physical frame; returns physical address or 0 on failure
uint32_t frame_alloc() {
    for (uint32_t i = 0; i < nframes; ++i) {
        if (!frame_test(i)) {
            frame_set(i);
            return i * 4096;
        }
    }
    return 0; // no free frames
}

// free a previously allocated 4 KiB physical frame
void frame_free(uint32_t paddr) {
    if (paddr % 4096 != 0) return;
    uint32_t idx = paddr / 4096;
    if (idx < nframes) {
        frame_clear(idx);
    }
}

// --- simple kernel heap (bump allocator) ---
static uintptr_t heap_ptr = 0;

// allocate size bytes from the kernel heap; no freeing. reserves underlying frames.
void* kmalloc(size_t size) {
    if (!heap_ptr) {
        // heap starts just after the frame bitmap
        heap_ptr = ((uintptr_t)frame_bitmap + bitmap_size + 7) & ~7u;
    }
    // align size to 8 bytes
    size = (size + 7) & ~7u;
    uintptr_t old = heap_ptr;
    uintptr_t new_ptr = heap_ptr + size;
    // reserve any frames spanned by this allocation
    uintptr_t start_page = old & ~4095u;
    uintptr_t end_page = (new_ptr + 4095) & ~4095u;
    for (uintptr_t p = start_page; p < end_page; p += 4096) {
        frame_set(p / 4096);
    }
    heap_ptr = new_ptr;
    return (void*)(uintptr_t)old;
}

// return number of physical frames detected
uint32_t memory_get_nframes(void) {
    return nframes;
}