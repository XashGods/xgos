#ifndef CONSOLE_H
#define CONSOLE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    void kputc(char c);
    void kprintf(const char* format, ...);

    // panic message and halt
    void panic(const char* msg);

    typedef enum {
        VGA_COLOR_BLACK = 0,
        VGA_COLOR_BLUE = 1,
        VGA_COLOR_GREEN = 2,
        VGA_COLOR_CYAN = 3,
        VGA_COLOR_RED = 4,
        VGA_COLOR_MAGENTA = 5,
        VGA_COLOR_BROWN = 6,
        VGA_COLOR_LIGHT_GREY = 7,
        VGA_COLOR_DARK_GREY = 8,
        VGA_COLOR_LIGHT_BLUE = 9,
        VGA_COLOR_LIGHT_GREEN = 10,
        VGA_COLOR_LIGHT_CYAN = 11,
        VGA_COLOR_LIGHT_RED = 12,
        VGA_COLOR_PINK = 13,
        VGA_COLOR_YELLOW = 14,
        VGA_COLOR_WHITE = 15
    } vga_color;

    uint8_t vga_make_color(vga_color fg, vga_color bg);
    void set_color(uint8_t color);
    void clear();
    void set_cursor(size_t row, size_t col);
    void enable_cursor();
    void disable_cursor();

#ifdef __cplusplus
}
#endif

#endif // CONSOLE_H