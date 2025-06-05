#include "paging.h"
#include "memory.h"
#include "console.h"
#include <stdint.h>

// 64-bit page table entry flags
#define PAGE_PRESENT    0x001
#define PAGE_RW         0x002
#define PAGE_USER       0x004
#define PAGE_WRITETHROUGH 0x008
#define PAGE_CACHE_DISABLE 0x010
#define PAGE_ACCESSED   0x020
#define PAGE_DIRTY      0x040
#define PAGE_HUGE       0x080
#define PAGE_GLOBAL     0x100
#define PAGE_NO_EXECUTE 0x8000000000000000ULL

// structure for 64-bit page table entries
typedef uint64_t page_entry_t;

// global pointer to PML4 table
static page_entry_t *pml4_table = nullptr;

// get a page table entry pointer
// static page_entry_t* get_page_entry(page_entry_t* table, int index) {
//     return &table[index];
// }

// get physical address from page entry
static uint64_t get_phys_addr(page_entry_t entry) {
    return entry & 0x000FFFFFFFFFF000ULL;
}

// allocate and zero a page table
static page_entry_t *alloc_page_table() {
    uint64_t frame = frame_alloc();
    if (!frame) {
        kprintf("paging: failed to allocate page table frame\n");
        return nullptr;
    }

    page_entry_t *table = (page_entry_t *) frame;
    // zero the table
    for (int i = 0; i < 512; i++) {
        table[i] = 0;
    }

    return table;
}

extern "C" void paging_init(void) {
    kprintf("paging_init: setting up 64-bit paging structures\n");

    // get current PML4 table (set up in boot.asm)
    uint64_t cr3_value;
    asm volatile("mov %%cr3, %0" : "=r"(cr3_value));
    pml4_table = (page_entry_t *) cr3_value;

    kprintf("paging_init: using PML4 at 0x%lx\n", (uint64_t) pml4_table);

    // the basic identity mapping is already set up in boot.asm using 2MB pages

    uint64_t total_frames = memory_get_nframes();
    kprintf("paging_init: managing %lu frames of memory\n", total_frames);

    // map additional memory regions if needed
    // for rn 2MB identity mapping from boot should be sufficient

    kprintf("paging_init: 64-bit paging initialized\n");
}

// map a virtual address to a physical address with given flags
void paging_map_page(uint64_t virt_addr, uint64_t phys_addr, uint64_t flags) {
    if (!pml4_table) {
        kprintf("paging_map_page: PML4 not initialized\n");
        return;
    }

    // extract indices for 4-level paging
    int pml4_index = (virt_addr >> 39) & 0x1FF;
    int pdpt_index = (virt_addr >> 30) & 0x1FF;
    int pd_index = (virt_addr >> 21) & 0x1FF;
    int pt_index = (virt_addr >> 12) & 0x1FF;

    // get or create PDPT
    page_entry_t *pdpt_table;
    if (!(pml4_table[pml4_index] & PAGE_PRESENT)) {
        pdpt_table = alloc_page_table();
        if (!pdpt_table) return;
        pml4_table[pml4_index] = (uint64_t) pdpt_table | PAGE_PRESENT | PAGE_RW;
    } else {
        pdpt_table = (page_entry_t *) get_phys_addr(pml4_table[pml4_index]);
    }

    // get or create PD
    page_entry_t *pd_table;
    if (!(pdpt_table[pdpt_index] & PAGE_PRESENT)) {
        pd_table = alloc_page_table();
        if (!pd_table) return;
        pdpt_table[pdpt_index] = (uint64_t) pd_table | PAGE_PRESENT | PAGE_RW;
    } else {
        pd_table = (page_entry_t *) get_phys_addr(pdpt_table[pdpt_index]);
    }

    // get or create PT
    page_entry_t *pt_table;
    if (!(pd_table[pd_index] & PAGE_PRESENT)) {
        pt_table = alloc_page_table();
        if (!pt_table) return;
        pd_table[pd_index] = (uint64_t) pt_table | PAGE_PRESENT | PAGE_RW;
    } else {
        pt_table = (page_entry_t *) get_phys_addr(pd_table[pd_index]);
    }

    // set the final page table entry
    pt_table[pt_index] = (phys_addr & 0x000FFFFFFFFFF000ULL) | flags;

    // invalidate TLB for this page
    asm volatile("invlpg (%0)" : : "r"(virt_addr) : "memory");
}

// unmap a virtual address
void paging_unmap_page(uint64_t virt_addr) {
    if (!pml4_table) return;

    int pml4_index = (virt_addr >> 39) & 0x1FF;
    int pdpt_index = (virt_addr >> 30) & 0x1FF;
    int pd_index = (virt_addr >> 21) & 0x1FF;
    int pt_index = (virt_addr >> 12) & 0x1FF;

    if (!(pml4_table[pml4_index] & PAGE_PRESENT)) return;
    page_entry_t *pdpt_table = (page_entry_t *) get_phys_addr(pml4_table[pml4_index]);

    if (!(pdpt_table[pdpt_index] & PAGE_PRESENT)) return;
    page_entry_t *pd_table = (page_entry_t *) get_phys_addr(pdpt_table[pdpt_index]);

    if (!(pd_table[pd_index] & PAGE_PRESENT)) return;
    page_entry_t *pt_table = (page_entry_t *) get_phys_addr(pd_table[pd_index]);

    // clear the page table entry
    pt_table[pt_index] = 0;

    // invalidate TLB for this page
    asm volatile("invlpg (%0)" : : "r"(virt_addr) : "memory");
}

// get physical address for a virtual address (page table walk)
uint64_t paging_get_physical(uint64_t virt_addr) {
    if (!pml4_table) return 0;

    int pml4_index = (virt_addr >> 39) & 0x1FF;
    int pdpt_index = (virt_addr >> 30) & 0x1FF;
    int pd_index = (virt_addr >> 21) & 0x1FF;
    int pt_index = (virt_addr >> 12) & 0x1FF;

    if (!(pml4_table[pml4_index] & PAGE_PRESENT)) return 0;
    page_entry_t *pdpt_table = (page_entry_t *) get_phys_addr(pml4_table[pml4_index]);

    if (!(pdpt_table[pdpt_index] & PAGE_PRESENT)) return 0;
    page_entry_t *pd_table = (page_entry_t *) get_phys_addr(pdpt_table[pdpt_index]);

    // check for 1GB pages
    if (pdpt_table[pdpt_index] & PAGE_HUGE) {
        return get_phys_addr(pdpt_table[pdpt_index]) + (virt_addr & 0x3FFFFFFF);
    }

    if (!(pd_table[pd_index] & PAGE_PRESENT)) return 0;

    // check for 2MB pages
    if (pd_table[pd_index] & PAGE_HUGE) {
        return get_phys_addr(pd_table[pd_index]) + (virt_addr & 0x1FFFFF);
    }

    page_entry_t *pt_table = (page_entry_t *) get_phys_addr(pd_table[pd_index]);

    if (!(pt_table[pt_index] & PAGE_PRESENT)) return 0;

    return get_phys_addr(pt_table[pt_index]) + (virt_addr & 0xFFF);
}

// map framebuffer memory region to virtual memory with proper flags
int paging_map_framebuffer(uint64_t phys_addr, uint64_t size) {
    if (!pml4_table) {
        kprintf("paging_map_framebuffer: PML4 not initialized\n");
        return -1;
    }

    // round size up to page boundary
    size = (size + 4095) & ~4095ULL;

    // map framebuffer with identity mapping (virtual = physical)
    uint64_t flags = PAGE_PRESENT | PAGE_RW | PAGE_CACHE_DISABLE;

    kprintf("paging: mapping framebuffer 0x%lx size 0x%lx\n", phys_addr, size);

    for (uint64_t offset = 0; offset < size; offset += 4096) {
        uint64_t addr = phys_addr + offset;
        paging_map_page(addr, addr, flags);
    }

    kprintf("paging: framebuffer mapping complete\n");
    return 0;
}
