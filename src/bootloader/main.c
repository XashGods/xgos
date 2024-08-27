#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_initialize(void) 
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = 15 | 0 << 4; // White on black
    terminal_buffer = (uint16_t*) 0xB8000;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = (uint16_t) ' ' | (uint16_t) terminal_color << 8;
        }
    }
}

void terminal_putchar(char c) 
{
    const size_t index = terminal_row * VGA_WIDTH + terminal_column;
    terminal_buffer[index] = (uint16_t) c | (uint16_t) terminal_color << 8;
    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT)
            terminal_row = 0;
    }
}

void terminal_writestring(const char* data) 
{
    for (size_t i = 0; data[i] != '\0'; i++)
        terminal_putchar(data[i]);
}

void kernel_main(void) 
{
    terminal_initialize();
    terminal_writestring("Hello, kernel World!");
}