#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// VESA Mode Info Structure (matches VBE specification)
typedef struct {
    uint16_t attributes; // Mode attributes
    uint8_t window_a; // Window A attributes
    uint8_t window_b; // Window B attributes
    uint16_t granularity; // Window granularity
    uint16_t window_size; // Window size
    uint16_t segment_a; // Window A segment
    uint16_t segment_b; // Window B segment
    uint32_t win_func_ptr; // Real mode pointer to window function
    uint16_t pitch; // Bytes per scan line
    uint16_t width; // Horizontal resolution
    uint16_t height; // Vertical resolution
    uint8_t w_char; // Character cell width
    uint8_t y_char; // Character cell height
    uint8_t planes; // Number of memory planes
    uint8_t bpp; // Bits per pixel
    uint8_t banks; // Number of banks
    uint8_t memory_model; // Memory model type
    uint8_t bank_size; // Bank size in KB
    uint8_t image_pages; // Number of images pages
    uint8_t reserved0;
    uint8_t red_mask; // Size of direct color red mask
    uint8_t red_position; // Bit position of red mask
    uint8_t green_mask; // Size of direct color green mask
    uint8_t green_position; // Bit position of green mask
    uint8_t blue_mask; // Size of direct color blue mask
    uint8_t blue_position; // Bit position of blue mask
    uint8_t reserved_mask; // Size of direct color reserved mask
    uint8_t reserved_position; // Bit position of reserved mask
    uint8_t direct_color_attributes; // Direct color mode attributes
    uint32_t framebuffer; // Physical address for LFB
    uint32_t off_screen_mem_off;
    uint16_t off_screen_mem_size;
    uint8_t reserved1[206];
} __attribute__((packed)) vbe_mode_info_t;

// Graphics context structure
typedef struct {
    uint32_t *framebuffer; // Virtual address of framebuffer
    uint32_t width; // Screen width in pixels
    uint32_t height; // Screen height in pixels
    uint32_t pitch; // Bytes per scanline
    uint32_t bpp; // Bits per pixel
    uint32_t red_shift; // Red component bit shift
    uint32_t green_shift; // Green component bit shift
    uint32_t blue_shift; // Blue component bit shift
    uint32_t red_mask; // Red component mask
    uint32_t green_mask; // Green component mask
    uint32_t blue_mask; // Blue component mask
    uint8_t initialized; // Whether graphics is initialized
} graphics_context_t;

// Color structure for convenience
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
} color_t;

// Predefined colors
extern const color_t COLOR_BLACK;
extern const color_t COLOR_WHITE;
extern const color_t COLOR_RED;
extern const color_t COLOR_GREEN;
extern const color_t COLOR_BLUE;
extern const color_t COLOR_YELLOW;
extern const color_t COLOR_CYAN;
extern const color_t COLOR_MAGENTA;
extern const color_t COLOR_GRAY;
extern const color_t COLOR_DARK_GRAY;

// graphics initialization and management
int graphics_init(vbe_mode_info_t *mode_info);

int graphics_init_simple(uint32_t *framebuffer, uint32_t width, uint32_t height, uint32_t pitch, uint8_t bpp);

void graphics_cleanup(void);

graphics_context_t *graphics_get_context(void);

int graphics_test_framebuffer(void);

// basic drawing primitives
void graphics_put_pixel(uint32_t x, uint32_t y, color_t color);

color_t graphics_get_pixel(uint32_t x, uint32_t y);

void graphics_clear_screen(color_t color);

void graphics_fill_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, color_t color);

void graphics_draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, color_t color);

// line drawing
void graphics_draw_line(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, color_t color);

void graphics_draw_horizontal_line(uint32_t x, uint32_t y, uint32_t width, color_t color);

void graphics_draw_vertical_line(uint32_t x, uint32_t y, uint32_t height, color_t color);

// utility functions
uint32_t graphics_color_to_pixel(color_t color);

color_t graphics_pixel_to_color(uint32_t pixel);

void graphics_swap_buffers(void);

// text rendering (basic bitmap font)
void graphics_draw_char(uint32_t x, uint32_t y, char c, color_t fg, color_t bg);

void graphics_draw_string(uint32_t x, uint32_t y, const char *str, color_t fg, color_t bg);

// advanced drawing functions
void graphics_draw_circle(uint32_t cx, uint32_t cy, uint32_t radius, color_t color);

void graphics_fill_circle(uint32_t cx, uint32_t cy, uint32_t radius, color_t color);

// animation functions
void graphics_delay(uint32_t cycles);

#ifdef __cplusplus
}
#endif

#endif // GRAPHICS_H
