#include "console.h"
#include <stdarg.h>
#include <stdint.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static volatile uint16_t* const VGA_BUFFER = (volatile uint16_t*)0xB8000;
static size_t cursor_row = 0;
static size_t cursor_col = 0;

void kputc(char c) {
    if (c == '\n') {
        cursor_col = 0;
        cursor_row++;
    } else {
        size_t idx = cursor_row * VGA_WIDTH + cursor_col;
        VGA_BUFFER[idx] = (uint16_t)c | (uint16_t)(0x07 << 8);
        cursor_col++;
        if (cursor_col >= VGA_WIDTH) {
            cursor_col = 0;
            cursor_row++;
        }
    }
    if (cursor_row >= VGA_HEIGHT) {
        // scroll up
        for (size_t y = 1; y < VGA_HEIGHT; ++y) {
            for (size_t x = 0; x < VGA_WIDTH; ++x) {
                VGA_BUFFER[(y - 1) * VGA_WIDTH + x] = VGA_BUFFER[y * VGA_WIDTH + x];
            }
        }
        // clear last line
        for (size_t x = 0; x < VGA_WIDTH; ++x) {
            VGA_BUFFER[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = (uint16_t)' ' | (uint16_t)(0x07 << 8);
        }
        cursor_row = VGA_HEIGHT - 1;
    }
}

void kprintf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    for (const char* p = format; *p; ++p) {
        if (*p == '%') {
            ++p;
            switch (*p) {
                case '%': kputc('%'); break;
                case 'c': kputc((char)va_arg(args, int)); break;
                case 's': {
                    const char* s = va_arg(args, const char*);
                    while (*s) kputc(*s++);
                } break;
                case 'd':
                case 'i': {
                    int num = va_arg(args, int);
                    if (num < 0) { kputc('-'); num = -num; }
                    char buf[12];
                    int len = 0;
                    unsigned int v = (unsigned int)num;
                    do {
                        buf[len++] = '0' + (v % 10);
                        v /= 10;
                    } while (v);
                    while (len--) kputc(buf[len]);
                } break;
                case 'u': {
                    unsigned int v = va_arg(args, unsigned int);
                    char buf[11];
                    int len = 0;
                    do {
                        buf[len++] = '0' + (v % 10);
                        v /= 10;
                    } while (v);
                    while (len--) kputc(buf[len]);
                } break;
                case 'x':
                case 'X': {
                    unsigned int v = va_arg(args, unsigned int);
                    char buf[9];
                    int len = 0;
                    const char* digits = (*p == 'x') ? "0123456789abcdef" : "0123456789ABCDEF";
                    do {
                        buf[len++] = digits[v % 16];
                        v /= 16;
                    } while (v);
                    while (len--) kputc(buf[len]);
                } break;
                case 'p': {
                    unsigned long ptr = va_arg(args, unsigned long);
                    kprintf("0x%x", (unsigned int)ptr);
                } break;
                default:
                    kputc('%');
                    kputc(*p);
                    break;
            }
        } else {
            kputc(*p);
        }
    }
    va_end(args);
}

// panic message and halt
void panic(const char* msg) {
    kprintf("PANIC: %s\n", msg);
    for (;;) {}
}