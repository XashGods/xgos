#ifndef CONSOLE_H
#define CONSOLE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

    void kputc(char c);
    void kprintf(const char* format, ...);

    // panic message and halt
    void panic(const char* msg);

#ifdef __cplusplus
}
#endif

#endif // CONSOLE_H