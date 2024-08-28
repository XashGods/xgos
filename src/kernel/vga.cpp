#include "kernel.h"
#include "vga.hpp"

void CVGA::Initialize(void) 
{
    this->terminal_row = 0;
    this->terminal_column = 0;
    this->terminal_color = 15 | 0 << 4; // White on black
    this->terminal_buffer = (uint16_t*) 0xB8000;
    for (size_t y = 0; y < this->VGA_HEIGHT; y++) {
        for (size_t x = 0; x < this->VGA_WIDTH; x++) {
            const size_t index = y * this->VGA_WIDTH + x;
            this->terminal_buffer[index] = (uint16_t) ' ' | (uint16_t) this->terminal_color << 8;
        }
    }
}

void CVGA::PutChar(char c) 
{
    if (c == '\n') {
        this->terminal_column = 0;
        if (++terminal_row == this->VGA_HEIGHT)
            this->terminal_row = 0;
    } else {
        const size_t index = this->terminal_row * this->VGA_WIDTH + this->terminal_column;
        this->terminal_buffer[index] = (uint16_t) c | (uint16_t) this->terminal_color << 8;
        if (++this->terminal_column == this->VGA_WIDTH) {
            this->terminal_column = 0;
            if (++this->terminal_row == this->VGA_HEIGHT)
                this->terminal_row = 0;
        }
    }
}

void CVGA::WriteString(const char* data) 
{
    for (size_t i = 0; data[i] != '\0'; i++)
        this->PutChar(data[i]);
}