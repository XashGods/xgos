#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "kernel.h"

class CVGA
{
    public:
        void Initialize();
        void PutChar(char c);
        void WriteString(const char *data);
    private:
        static const size_t VGA_WIDTH = 80;
        static const size_t VGA_HEIGHT = 25;
        uint16_t *terminal_buffer;
        uint8_t terminal_color;
        size_t terminal_column;
        size_t terminal_row;
};