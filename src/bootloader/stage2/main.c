#include <stdint.h>
#include <stdio.h>

extern void cstart_(uint8_t drive)
{
    puts("Stage 2 bootloader started");

    for (;;);
}