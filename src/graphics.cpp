#include "graphics.h"
#include "memory.h"
#include "console.h"
#include "paging.h"
#include <stdint.h>
#include <stddef.h>

static graphics_context_t g_graphics_ctx = {
    .framebuffer = NULL,
    .width = 0,
    .height = 0,
    .pitch = 0,
    .bpp = 0,
    .red_shift = 0,
    .green_shift = 0,
    .blue_shift = 0,
    .red_mask = 0,
    .green_mask = 0,
    .blue_mask = 0,
    .initialized = 0
};

const color_t COLOR_BLACK = {0x00, 0x00, 0x00, 0xFF};
const color_t COLOR_WHITE = {0xFF, 0xFF, 0xFF, 0xFF};
const color_t COLOR_RED = {0xFF, 0x00, 0x00, 0xFF};
const color_t COLOR_GREEN = {0x00, 0xFF, 0x00, 0xFF};
const color_t COLOR_BLUE = {0x00, 0x00, 0xFF, 0xFF};
const color_t COLOR_YELLOW = {0xFF, 0xFF, 0x00, 0xFF};
const color_t COLOR_CYAN = {0x00, 0xFF, 0xFF, 0xFF};
const color_t COLOR_MAGENTA = {0xFF, 0x00, 0xFF, 0xFF};
const color_t COLOR_GRAY = {0x80, 0x80, 0x80, 0xFF};
const color_t COLOR_DARK_GRAY = {0x40, 0x40, 0x40, 0xFF};

static const uint8_t font_8x8[95][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0020 (space)
    {0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00}, // U+0021 (!)
    {0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0022 (")
    {0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00}, // U+0023 (#)
    {0x0C, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x0C, 0x00}, // U+0024 ($)
    {0x00, 0x63, 0x33, 0x18, 0x0C, 0x66, 0x63, 0x00}, // U+0025 (%)
    {0x1C, 0x36, 0x1C, 0x6E, 0x3B, 0x33, 0x6E, 0x00}, // U+0026 (&)
    {0x06, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0027 (')
    {0x18, 0x0C, 0x06, 0x06, 0x06, 0x0C, 0x18, 0x00}, // U+0028 (()
    {0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06, 0x00}, // U+0029 ())
    {0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00}, // U+002A (*)
    {0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x00}, // U+002B (+)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x06}, // U+002C (,)
    {0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00}, // U+002D (-)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00}, // U+002E (.)
    {0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00}, // U+002F (/)
    {0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00}, // U+0030 (0)
    {0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00}, // U+0031 (1)
    {0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00}, // U+0032 (2)
    {0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00}, // U+0033 (3)
    {0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00}, // U+0034 (4)
    {0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00}, // U+0035 (5)
    {0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00}, // U+0036 (6)
    {0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00}, // U+0037 (7)
    {0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00}, // U+0038 (8)
    {0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00}, // U+0039 (9)
    {0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x00}, // U+003A (:)
    {0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x06}, // U+003B (;)
    {0x18, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x18, 0x00}, // U+003C (<)
    {0x00, 0x00, 0x3F, 0x00, 0x00, 0x3F, 0x00, 0x00}, // U+003D (=)
    {0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00}, // U+003E (>)
    {0x1E, 0x33, 0x30, 0x18, 0x0C, 0x00, 0x0C, 0x00}, // U+003F (?)
    {0x3E, 0x63, 0x7B, 0x7B, 0x7B, 0x03, 0x1E, 0x00}, // U+0040 (@)
    {0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00}, // U+0041 (A)
    {0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F, 0x00}, // U+0042 (B)
    {0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C, 0x00}, // U+0043 (C)
    {0x1F, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1F, 0x00}, // U+0044 (D)
    {0x7F, 0x46, 0x16, 0x1E, 0x16, 0x46, 0x7F, 0x00}, // U+0045 (E)
    {0x7F, 0x46, 0x16, 0x1E, 0x16, 0x06, 0x0F, 0x00}, // U+0046 (F)
    {0x3C, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7C, 0x00}, // U+0047 (G)
    {0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00}, // U+0048 (H)
    {0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // U+0049 (I)
    {0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E, 0x00}, // U+004A (J)
    {0x67, 0x66, 0x36, 0x1E, 0x36, 0x66, 0x67, 0x00}, // U+004B (K)
    {0x0F, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7F, 0x00}, // U+004C (L)
    {0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00}, // U+004D (M)
    {0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00}, // U+004E (N)
    {0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00}, // U+004F (O)
    {0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00}, // U+0050 (P)
    {0x1E, 0x33, 0x33, 0x33, 0x3B, 0x1E, 0x38, 0x00}, // U+0051 (Q)
    {0x3F, 0x66, 0x66, 0x3E, 0x36, 0x66, 0x67, 0x00}, // U+0052 (R)
    {0x1E, 0x33, 0x07, 0x0E, 0x38, 0x33, 0x1E, 0x00}, // U+0053 (S)
    {0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // U+0054 (T)
    {0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F, 0x00}, // U+0055 (U)
    {0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00}, // U+0056 (V)
    {0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00}, // U+0057 (W)
    {0x63, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x00}, // U+0058 (X)
    {0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x1E, 0x00}, // U+0059 (Y)
    {0x7F, 0x63, 0x31, 0x18, 0x4C, 0x66, 0x7F, 0x00}, // U+005A (Z)
    {0x1E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x1E, 0x00}, // U+005B ([)
    {0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00}, // U+005C (\)
    {0x1E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1E, 0x00}, // U+005D (])
    {0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00}, // U+005E (^)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF}, // U+005F (_)
    {0x0C, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0060 (`)
    {0x00, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x6E, 0x00}, // U+0061 (a)
    {0x07, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3B, 0x00}, // U+0062 (b)
    {0x00, 0x00, 0x1E, 0x33, 0x03, 0x33, 0x1E, 0x00}, // U+0063 (c)
    {0x38, 0x30, 0x30, 0x3e, 0x33, 0x33, 0x6E, 0x00}, // U+0064 (d)
    {0x00, 0x00, 0x1E, 0x33, 0x3f, 0x03, 0x1E, 0x00}, // U+0065 (e)
    {0x1C, 0x36, 0x06, 0x0f, 0x06, 0x06, 0x0F, 0x00}, // U+0066 (f)
    {0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x1F}, // U+0067 (g)
    {0x07, 0x06, 0x36, 0x6E, 0x66, 0x66, 0x67, 0x00}, // U+0068 (h)
    {0x0C, 0x00, 0x0E, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // U+0069 (i)
    {0x30, 0x00, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E}, // U+006A (j)
    {0x07, 0x06, 0x66, 0x36, 0x1E, 0x36, 0x67, 0x00}, // U+006B (k)
    {0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // U+006C (l)
    {0x00, 0x00, 0x33, 0x7F, 0x7F, 0x6B, 0x63, 0x00}, // U+006D (m)
    {0x00, 0x00, 0x1F, 0x33, 0x33, 0x33, 0x33, 0x00}, // U+006E (n)
    {0x00, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x1E, 0x00}, // U+006F (o)
    {0x00, 0x00, 0x3B, 0x66, 0x66, 0x3E, 0x06, 0x0F}, // U+0070 (p)
    {0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x78}, // U+0071 (q)
    {0x00, 0x00, 0x3B, 0x6E, 0x66, 0x06, 0x0F, 0x00}, // U+0072 (r)
    {0x00, 0x00, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x00}, // U+0073 (s)
    {0x08, 0x0C, 0x3E, 0x0C, 0x0C, 0x2C, 0x18, 0x00}, // U+0074 (t)
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x6E, 0x00}, // U+0075 (u)
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00}, // U+0076 (v)
    {0x00, 0x00, 0x63, 0x6B, 0x7F, 0x7F, 0x36, 0x00}, // U+0077 (w)
    {0x00, 0x00, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x00}, // U+0078 (x)
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x3E, 0x30, 0x1F}, // U+0079 (y)
    {0x00, 0x00, 0x3F, 0x19, 0x0C, 0x26, 0x3F, 0x00}, // U+007A (z)
    {0x38, 0x0C, 0x0C, 0x07, 0x0C, 0x0C, 0x38, 0x00}, // U+007B ({)
    {0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00}, // U+007C (|)
    {0x07, 0x0C, 0x0C, 0x38, 0x0C, 0x0C, 0x07, 0x00}, // U+007D (})
    {0x6E, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+007E (~)
};


int graphics_init(vbe_mode_info_t *mode_info) {
    if (!mode_info) {
        kprintf("Graphics: Invalid mode info\n");
        return -1;
    }

    // if mode supports linear framebuffer
    if (!(mode_info->attributes & 0x80)) {
        kprintf("Graphics: Mode does not support linear framebuffer\n");
        return -1;
    }

    // if it's a packed pixel or direct color mode
    if (mode_info->memory_model != 4 && mode_info->memory_model != 6) {
        kprintf("Graphics: Unsupported memory model: %d\n", mode_info->memory_model);
        return -1;
    }

    // only support 32-bit color for now
    if (mode_info->bpp != 32) {
        kprintf("Graphics: Only 32-bit color modes supported, got %d bpp\n", mode_info->bpp);
        return -1;
    }

    // calculate framebuffer size
    uint32_t fb_size = mode_info->pitch * mode_info->height;
    kprintf("Graphics: Framebuffer at 0x%lx, size %lu bytes\n",
            (unsigned long) mode_info->framebuffer, (unsigned long) fb_size);

    // map the framebuffer to virtual memory
    // for rn we'll use identity mapping for simplicity
    // in a more advanced implementation, we will allocate virtual pages
    g_graphics_ctx.framebuffer = (uint32_t *) (uintptr_t) mode_info->framebuffer;

    // set up graphics context
    g_graphics_ctx.width = mode_info->width;
    g_graphics_ctx.height = mode_info->height;
    g_graphics_ctx.pitch = mode_info->pitch;
    g_graphics_ctx.bpp = mode_info->bpp;

    // calculate color masks and shifts for 32-bit RGBA/BGRA
    g_graphics_ctx.red_shift = mode_info->red_position;
    g_graphics_ctx.green_shift = mode_info->green_position;
    g_graphics_ctx.blue_shift = mode_info->blue_position;

    g_graphics_ctx.red_mask = (1U << mode_info->red_mask) - 1;
    g_graphics_ctx.green_mask = (1U << mode_info->green_mask) - 1;
    g_graphics_ctx.blue_mask = (1U << mode_info->blue_mask) - 1;

    g_graphics_ctx.initialized = 1;

    kprintf("Graphics: Initialized %dx%d %d bpp framebuffer\n",
            g_graphics_ctx.width, g_graphics_ctx.height, g_graphics_ctx.bpp);
    kprintf("Graphics: Color masks - R:%d@%d G:%d@%d B:%d@%d\n",
            mode_info->red_mask, mode_info->red_position,
            mode_info->green_mask, mode_info->green_position,
            mode_info->blue_mask, mode_info->blue_position);

    // clear screen
    graphics_clear_screen(COLOR_BLACK);

    return 0;
}

// initialize graphics subsystem with simple parameters (for multiboot framebuffer)
int graphics_init_simple(uint32_t *framebuffer, uint32_t width, uint32_t height, uint32_t pitch, uint8_t bpp) {
    if (!framebuffer) {
        kprintf("Graphics: Invalid framebuffer pointer\n");
        return -1;
    }

    // only support 32-bit color for now
    if (bpp != 32) {
        kprintf("Graphics: Only 32-bit color modes supported, got %d bpp\n", bpp);
        return -1;
    }

    // validate framebuffer address range - it should be above 1MB
    uint64_t fb_addr = (uint64_t) (uintptr_t) framebuffer;
    if (fb_addr < 0x100000) {
        kprintf("Graphics: Framebuffer address 0x%lx too low, invalid\n", fb_addr);
        return -1;
    }

    // calculate framebuffer size and check if it's reasonable
    uint64_t fb_size = (uint64_t) pitch * height;
    if (fb_size > 0x10000000) {
        // 256MB max
        kprintf("Graphics: Framebuffer size %lu bytes too large\n", fb_size);
        return -1;
    }

    kprintf("Graphics: Framebuffer at 0x%lx, size %lu bytes\n", fb_addr, fb_size);

    // set up graphics context
    g_graphics_ctx.framebuffer = framebuffer;
    g_graphics_ctx.width = width;
    g_graphics_ctx.height = height;
    g_graphics_ctx.pitch = pitch;
    g_graphics_ctx.bpp = bpp;

    // assume standard 32-bit RGBA format for multiboot framebuffer
    g_graphics_ctx.red_shift = 16;
    g_graphics_ctx.green_shift = 8;
    g_graphics_ctx.blue_shift = 0;

    g_graphics_ctx.red_mask = 0xFF;
    g_graphics_ctx.green_mask = 0xFF;
    g_graphics_ctx.blue_mask = 0xFF;

    g_graphics_ctx.initialized = 1;

    kprintf("Graphics: Initialized %dx%d %d bpp framebuffer\n",
            g_graphics_ctx.width, g_graphics_ctx.height, g_graphics_ctx.bpp);

    // WARNING: do not clear screen immediately - this might cause page fault
    // graphics_clear_screen(COLOR_BLACK);

    return 0;
}

// get the current graphics context
graphics_context_t *graphics_get_context(void) {
    return g_graphics_ctx.initialized ? &g_graphics_ctx : NULL;
}

// convert color structure to pixel value
uint32_t graphics_color_to_pixel(color_t color) {
    if (!g_graphics_ctx.initialized) return 0;

    uint32_t pixel = 0;
    pixel |= ((uint32_t) (color.red & g_graphics_ctx.red_mask) << g_graphics_ctx.red_shift);
    pixel |= ((uint32_t) (color.green & g_graphics_ctx.green_mask) << g_graphics_ctx.green_shift);
    pixel |= ((uint32_t) (color.blue & g_graphics_ctx.blue_mask) << g_graphics_ctx.blue_shift);

    return pixel;
}

// convert pixel value to color structure
color_t graphics_pixel_to_color(uint32_t pixel) {
    color_t color = {.red = 0, .green = 0, .blue = 0, .alpha = 0};

    if (!g_graphics_ctx.initialized) return color;

    color.red = (pixel >> g_graphics_ctx.red_shift) & g_graphics_ctx.red_mask;
    color.green = (pixel >> g_graphics_ctx.green_shift) & g_graphics_ctx.green_mask;
    color.blue = (pixel >> g_graphics_ctx.blue_shift) & g_graphics_ctx.blue_mask;
    color.alpha = 255;

    return color;
}

// put a pixel at the specified coordinates
void graphics_put_pixel(uint32_t x, uint32_t y, color_t color) {
    if (!g_graphics_ctx.initialized || x >= g_graphics_ctx.width || y >= g_graphics_ctx.height) {
        return;
    }

    uint32_t pixel = graphics_color_to_pixel(color);
    uint32_t offset = (y * g_graphics_ctx.pitch / 4) + x;
    g_graphics_ctx.framebuffer[offset] = pixel;
}

// get a pixel at the specified coordinates
color_t graphics_get_pixel(uint32_t x, uint32_t y) {
    color_t black = COLOR_BLACK;

    if (!g_graphics_ctx.initialized || x >= g_graphics_ctx.width || y >= g_graphics_ctx.height) {
        return black;
    }

    uint32_t offset = (y * g_graphics_ctx.pitch / 4) + x;
    uint32_t pixel = g_graphics_ctx.framebuffer[offset];

    return graphics_pixel_to_color(pixel);
}

// clear the entire screen with the specified color
void graphics_clear_screen(color_t color) {
    if (!g_graphics_ctx.initialized) return;

    uint32_t pixel = graphics_color_to_pixel(color);
    uint32_t pixels_per_line = g_graphics_ctx.pitch / 4;

    for (uint32_t y = 0; y < g_graphics_ctx.height; y++) {
        for (uint32_t x = 0; x < g_graphics_ctx.width; x++) {
            g_graphics_ctx.framebuffer[y * pixels_per_line + x] = pixel;
        }
    }
}

// test framebuffer access by writing and reading a single pixel
// 0 on success, -1 on failure

int graphics_test_framebuffer(void) {
    if (!g_graphics_ctx.initialized) return -1;

    // try to write and read back a pixel at position (0,0)
    uint32_t *fb = g_graphics_ctx.framebuffer;
    uint32_t original = fb[0]; // save original value

    // write test pattern
    fb[0] = 0x12345678;

    // read back and verify
    uint32_t readback = fb[0];

    // restore original value
    fb[0] = original;

    return -1 + (readback == 0x12345678);
}

// fill a rectangle with the specified color
void graphics_fill_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, color_t color) {
    if (!g_graphics_ctx.initialized) return;

    uint32_t pixel = graphics_color_to_pixel(color);
    uint32_t pixels_per_line = g_graphics_ctx.pitch / 4;

    for (uint32_t row = y; row < y + height && row < g_graphics_ctx.height; row++) {
        for (uint32_t col = x; col < x + width && col < g_graphics_ctx.width; col++) {
            g_graphics_ctx.framebuffer[row * pixels_per_line + col] = pixel;
        }
    }
}

// draw a rectangle outline with the specified color
void graphics_draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, color_t color) {
    if (!g_graphics_ctx.initialized || width == 0 || height == 0) return;

    // top and bottom lines
    graphics_draw_horizontal_line(x, y, width, color);
    if (height > 1) {
        graphics_draw_horizontal_line(x, y + height - 1, width, color);
    }

    // left and right lines
    if (height > 2) {
        graphics_draw_vertical_line(x, y + 1, height - 2, color);
        if (width > 1) {
            graphics_draw_vertical_line(x + width - 1, y + 1, height - 2, color);
        }
    }
}

// draw a horizontal line
void graphics_draw_horizontal_line(uint32_t x, uint32_t y, uint32_t width, color_t color) {
    if (!g_graphics_ctx.initialized || y >= g_graphics_ctx.height) return;

    uint32_t pixel = graphics_color_to_pixel(color);
    uint32_t pixels_per_line = g_graphics_ctx.pitch / 4;
    uint32_t end_x = x + width;

    if (end_x > g_graphics_ctx.width) {
        end_x = g_graphics_ctx.width;
    }

    for (uint32_t col = x; col < end_x; col++) {
        g_graphics_ctx.framebuffer[y * pixels_per_line + col] = pixel;
    }
}

// draw a vertical line
void graphics_draw_vertical_line(uint32_t x, uint32_t y, uint32_t height, color_t color) {
    if (!g_graphics_ctx.initialized || x >= g_graphics_ctx.width) return;

    uint32_t pixel = graphics_color_to_pixel(color);
    uint32_t pixels_per_line = g_graphics_ctx.pitch / 4;
    uint32_t end_y = y + height;

    if (end_y > g_graphics_ctx.height) {
        end_y = g_graphics_ctx.height;
    }

    for (uint32_t row = y; row < end_y; row++) {
        g_graphics_ctx.framebuffer[row * pixels_per_line + x] = pixel;
    }
}

// draw a line using Bresenham's algorithm
void graphics_draw_line(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, color_t color) {
    if (!g_graphics_ctx.initialized) return;

    const int dx = x1 > x0 ? x1 - x0 : x0 - x1;
    const int dy = y1 > y0 ? y1 - y0 : y0 - y1;
    const int sx = x0 < x1 ? 1 : -1;
    const int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;

    while (1) {
        graphics_put_pixel(x0, y0, color);

        if (x0 == x1 && y0 == y1) break;

        const int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

// draw a character using the built-in font
void graphics_draw_char(uint32_t x, uint32_t y, char c, color_t fg, color_t bg) {
    if (!g_graphics_ctx.initialized || c < 32 || c > 126) return;

    const uint8_t *glyph = font_8x8[c - 32];
    // standard VGA font bit order: LSB to MSB corresponds to left to right pixels
    // bit masks for each column (left to right): {1,2,4,8,16,32,64,128}
    const int mask[8] = {1, 2, 4, 8, 16, 32, 64, 128};

    for (int row = 0; row < 8; row++) {
        uint8_t line = glyph[row];
        for (int col = 0; col < 8; col++) {
            color_t pixel_color = (line & mask[col]) ? fg : bg;
            graphics_put_pixel(x + col, y + row, pixel_color);
        }
    }
}

// draw a string using the built-in font
void graphics_draw_string(uint32_t x, uint32_t y, const char *str, color_t fg, color_t bg) {
    if (!g_graphics_ctx.initialized || !str) return;

    uint32_t pos_x = x;

    while (*str) {
        if (*str == '\n') {
            pos_x = x;
            y += 8;
        } else {
            graphics_draw_char(pos_x, y, *str, fg, bg);
            pos_x += 8;
        }
        str++;
    }
}

// cleanup graphics subsystem
void graphics_cleanup(void) {
    g_graphics_ctx.initialized = 0;
    g_graphics_ctx.framebuffer = NULL;
}

// simple delay function (busy wait)
void graphics_delay(uint32_t cycles) {
    for (volatile uint32_t i = 0; i < cycles; i++) {
        // busy wait
    }
}

// draw a filled circle using midpoint algorithm
void graphics_draw_circle(uint32_t cx, uint32_t cy, uint32_t radius, color_t color) {
    if (!g_graphics_ctx.initialized) return;

    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    // draw initial points
    graphics_put_pixel(cx + x, cy + y, color);
    graphics_put_pixel(cx - x, cy + y, color);
    graphics_put_pixel(cx + x, cy - y, color);
    graphics_put_pixel(cx - x, cy - y, color);
    graphics_put_pixel(cx + y, cy + x, color);
    graphics_put_pixel(cx - y, cy + x, color);
    graphics_put_pixel(cx + y, cy - x, color);
    graphics_put_pixel(cx - y, cy - x, color);

    while (y >= x) {
        x++;

        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }

        graphics_put_pixel(cx + x, cy + y, color);
        graphics_put_pixel(cx - x, cy + y, color);
        graphics_put_pixel(cx + x, cy - y, color);
        graphics_put_pixel(cx - x, cy - y, color);
        graphics_put_pixel(cx + y, cy + x, color);
        graphics_put_pixel(cx - y, cy + x, color);
        graphics_put_pixel(cx + y, cy - x, color);
        graphics_put_pixel(cx - y, cy - x, color);
    }
}

// draw a filled circle
void graphics_fill_circle(uint32_t cx, uint32_t cy, uint32_t radius, color_t color) {
    if (!g_graphics_ctx.initialized) return;

    for (int y = -radius; y <= (int) radius; y++) {
        for (int x = -radius; x <= (int) radius; x++) {
            if (x * x + y * y <= (int) (radius * radius)) {
                uint32_t px = cx + x;
                uint32_t py = cy + y;
                if (px < g_graphics_ctx.width && py < g_graphics_ctx.height) {
                    graphics_put_pixel(px, py, color);
                }
            }
        }
    }
}
