#include "console.h"
#include <stdarg.h>
#include <stdint.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static volatile uint16_t *const VGA_BUFFER = (volatile uint16_t *) 0xB8000;
static size_t cursor_row = 0;
static size_t cursor_col = 0;

static inline void outb(uint16_t port, uint8_t value) {
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static uint8_t current_color = (uint8_t) (0x07);

static void update_cursor(void) {
    uint16_t pos = (uint16_t) (cursor_row * VGA_WIDTH + cursor_col);
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) (pos >> 8));
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
}

void kputc(char c) {
    if (c == '\n') {
        cursor_col = 0;
        cursor_row++;
    } else {
        size_t idx = cursor_row * VGA_WIDTH + cursor_col;
        VGA_BUFFER[idx] = (uint16_t) c | (uint16_t) (current_color << 8);
        cursor_col++;
        if (cursor_col >= VGA_WIDTH) {
            cursor_col = 0;
            cursor_row++;
        }
    }
    if (cursor_row >= VGA_HEIGHT) {
        for (size_t y = 1; y < VGA_HEIGHT; ++y) {
            for (size_t x = 0; x < VGA_WIDTH; ++x) {
                VGA_BUFFER[(y - 1) * VGA_WIDTH + x] = VGA_BUFFER[y * VGA_WIDTH + x];
            }
        }
        for (size_t x = 0; x < VGA_WIDTH; ++x) {
            VGA_BUFFER[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = (uint16_t) ' ' | (uint16_t) (current_color << 8);
        }
        cursor_row = VGA_HEIGHT - 1;
    }
    update_cursor();
}

void kprintf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    for (const char *p = format; *p; ++p) {
        if (*p == '%') {
            ++p;
            switch (*p) {
                case '%': kputc('%');
                    break;
                case 'c': kputc((char) va_arg(args, int));
                    break;
                case 's': {
                    const char *s = va_arg(args, const char*);
                    while (*s) kputc(*s++);
                }
                break;
                case 'd':
                case 'i': {
                    int num = va_arg(args, int);
                    if (num < 0) {
                        kputc('-');
                        num = -num;
                    }
                    char buf[12];
                    int len = 0;
                    unsigned int v = (unsigned int) num;
                    do {
                        buf[len++] = '0' + (v % 10);
                        v /= 10;
                    } while (v);
                    while (len--) kputc(buf[len]);
                }
                break;
                case 'u': {
                    unsigned int v = va_arg(args, unsigned int);
                    char buf[11];
                    int len = 0;
                    do {
                        buf[len++] = '0' + (v % 10);
                        v /= 10;
                    } while (v);
                    while (len--) kputc(buf[len]);
                }
                break;
                case 'x':
                case 'X': {
                    unsigned int v = va_arg(args, unsigned int);
                    char buf[9];
                    int len = 0;
                    const char *digits = (*p == 'x') ? "0123456789abcdef" : "0123456789ABCDEF";
                    do {
                        buf[len++] = digits[v % 16];
                        v /= 16;
                    } while (v);
                    while (len--) kputc(buf[len]);
                }
                break;
                case 'l': {
                    ++p; // consume the 'l'
                    switch (*p) {
                        case 'u': {
                            unsigned long v = va_arg(args, unsigned long);
                            char buf[21];
                            int len = 0;
                            do {
                                buf[len++] = '0' + (v % 10);
                                v /= 10;
                            } while (v);
                            while (len--) kputc(buf[len]);
                        }
                        break;
                        case 'x': {
                            unsigned long v = va_arg(args, unsigned long);
                            char buf[17];
                            int len = 0;
                            const char *digits = "0123456789abcdef";
                            do {
                                buf[len++] = digits[v % 16];
                                v /= 16;
                            } while (v);
                            while (len--) kputc(buf[len]);
                        }
                        break;
                        case 'd': {
                            long num = va_arg(args, long);
                            if (num < 0) {
                                kputc('-');
                                num = -num;
                            }
                            char buf[21];
                            int len = 0;
                            unsigned long v = (unsigned long) num;
                            do {
                                buf[len++] = '0' + (v % 10);
                                v /= 10;
                            } while (v);
                            while (len--) kputc(buf[len]);
                        }
                        break;
                        default:
                            kputc('%');
                            kputc('l');
                            kputc(*p);
                            break;
                    }
                }
                break;
                case 'p': {
                    unsigned long ptr = va_arg(args, unsigned long);
                    kprintf("0x%lx", ptr);
                }
                break;
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
void panic(const char *msg) {
    kprintf("PANIC: %s\n", msg);
    for (;;) {
    }
}

uint8_t vga_make_color(vga_color fg, vga_color bg) {
    return (uint8_t) fg | (uint8_t) (bg << 4);
}

void set_color(uint8_t color) {
    current_color = color;
}

void clear() {
    for (size_t y = 0; y < VGA_HEIGHT; ++y) {
        for (size_t x = 0; x < VGA_WIDTH; ++x) {
            VGA_BUFFER[y * VGA_WIDTH + x] = (uint16_t) ' ' | (uint16_t) (current_color << 8);
        }
    }
    cursor_row = 0;
    cursor_col = 0;
    update_cursor();
}

void set_cursor(size_t row, size_t col) {
    cursor_row = row;
    cursor_col = col;
    update_cursor();
}

void enable_cursor() {
    outb(0x3D4, 0x0A);
    uint8_t val = inb(0x3D5) & ~0x20;
    outb(0x3D5, val);
}

void disable_cursor() {
    outb(0x3D4, 0x0A);
    uint8_t val = inb(0x3D5) | 0x20;
    outb(0x3D5, val);
}
