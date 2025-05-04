// Paging setup: identity-map all physical memory and enable x86 paging
#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// initialize paging: identity-map all detected physical memory,
// then enable CPU paging (set CR3 and CR0.PG).
void paging_init(void);

#ifdef __cplusplus
}
#endif

#endif // PAGING_H