// Paging setup: 64-bit paging with 4-level page tables
#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// basic paging is already enabled in boot.asm
// set 4-level page tables
void paging_init(void);

// map a virtual address to a physical address with given flags
void paging_map_page(uint64_t virt_addr, uint64_t phys_addr, uint64_t flags);

// unmap a virtual address
void paging_unmap_page(uint64_t virt_addr);

// get physical address for a virtual address (page table walk)
uint64_t paging_get_physical(uint64_t virt_addr);

#ifdef __cplusplus
}
#endif

#endif // PAGING_H
