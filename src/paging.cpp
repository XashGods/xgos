#include "paging.h"
#include "memory.h"
#include "console.h"
#include <stdint.h>

#define PAGE_PRESENT 0x1
#define PAGE_RW      0x2

extern "C" void paging_init(void) {
    uint32_t total_frames = memory_get_nframes();
    uint32_t num_pt = (total_frames + 1023) / 1024;

    // allocate a page for the page directory
    uint32_t pd_frame = frame_alloc();
    if (!pd_frame) panic("paging_init: failed to alloc page directory");
    uint32_t* pd = (uint32_t*)(uintptr_t)pd_frame;
    // clear the page directory
    for (int i = 0; i < 1024; ++i) pd[i] = 0;

    // allocate and fill each page table
    for (uint32_t i = 0; i < num_pt; ++i) {
        uint32_t pt_frame = frame_alloc();
        if (!pt_frame) panic("paging_init: failed to alloc page table");
        uint32_t* pt = (uint32_t*)(uintptr_t)pt_frame;
        // fill entries to identity-map physical pages
        for (uint32_t j = 0; j < 1024; ++j) {
            uint32_t page = (i << 22) | (j << 12);
            if ((page / 4096) < total_frames) {
                pt[j] = page | PAGE_PRESENT | PAGE_RW;
            } else {
                pt[j] = 0;
            }
        }
        // point the directory entry to this table
        pd[i] = pt_frame | PAGE_PRESENT | PAGE_RW;
    }

    // load page directory base register (CR3)
    asm volatile ("mov %0, %%cr3" :: "r" (pd_frame));
    // enable paging by setting the PG bit in CR0
    uint32_t cr0;
    asm volatile ("mov %%cr0, %0" : "=r" (cr0));
    cr0 |= 0x80000000;
    asm volatile ("mov %0, %%cr0" :: "r" (cr0));

    kprintf("paging_init: enabled, mapped %u frames\n", total_frames);
}