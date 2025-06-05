#include "graphics_demo.h"
#include "graphics.h"
#include <stdint.h>
#include "math.h"

// Animation: Bouncing ball
void graphics_animate_bouncing_ball(void) {
    graphics_context_t *ctx = graphics_get_context();
    if (!ctx || !ctx->initialized) return;

    int32_t x = 50;
    int32_t y = 50;
    int32_t vx = 2;
    int32_t vy = 2;
    uint32_t radius = 20;

    // animation colors
    color_t ball_colors[] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW, COLOR_CYAN, COLOR_MAGENTA};
    uint32_t color_index = 0;
    uint32_t frame_count = 0;

    for (int i = 0; i < 1000; i++) {
        // run for 1000 frames
        // clear screen
        graphics_clear_screen(COLOR_BLACK);

        // update ball position
        x += vx;
        y += vy;
        // get screen dimensions
        uint32_t width = ctx->width;
        uint32_t height = ctx->height;

        // bounce off walls
        if (x <= (int32_t) radius || x >= (int32_t) (width - radius)) {
            vx = -vx;
            color_index = (color_index + 1) % 6; // change color on bounce
        }
        if (y <= (int32_t) radius || y >= (int32_t) (height - radius)) {
            vy = -vy;
            color_index = (color_index + 1) % 6; // change color on bounce
        }

        // keep ball in bounds
        if (x < (int32_t) radius) x = radius;
        if (x > (int32_t) (width - radius)) x = width - radius;
        if (y < (int32_t) radius) y = radius;
        if (y > (int32_t) (height - radius)) y = height - radius;

        // draw ball
        graphics_fill_circle((uint32_t) x, (uint32_t) y, radius, ball_colors[color_index]);

        // draw frame counter
        frame_count++;
        graphics_draw_string(10, 10, "XG OS Graphics Demo", COLOR_WHITE, COLOR_BLACK);

        // simple frame rate control
        graphics_delay(500000); // adjust this value to control animation speed
    }
}

// color wave effect
void graphics_animate_color_wave(void) {
    graphics_context_t *ctx = graphics_get_context();
    if (!ctx || !ctx->initialized) return;

    uint32_t width = ctx->width;
    uint32_t height = ctx->height;

    for (int frame = 0; frame < 500; frame++) {
        for (uint32_t y = 0; y < height; y++) {
            for (uint32_t x = 0; x < width; x++) {
                // create wave pattern using integer math
                // scale down coordinates to avoid overflow
                int32_t wave1 = math_sin((x + frame * 2) / 4); // scaled down
                int32_t wave2 = math_sin((y + frame) / 3); // scaled down

                // combine waves (both are scaled by 1000)
                int32_t wave = (wave1 * wave2) / 1000; // result still scaled by 1000

                // convert wave to color (wave is -1000 to +1000)
                uint8_t red = (uint8_t) (128 + (wave * 127) / 1000);
                uint8_t green = (uint8_t) (128 + (math_sin(frame + x / 8) * 127) / 1000);
                uint8_t blue = (uint8_t) (128 + (math_cos(frame + y / 8) * 127) / 1000);

                color_t color = {red, green, blue, 255};
                graphics_put_pixel(x, y, color);
            }
        }
        // Frame rate control
        graphics_delay(100000);
    }
}

// Rotating rectangles
void graphics_animate_rotating_rects(void) {
    graphics_context_t *ctx = graphics_get_context();
    if (!ctx || !ctx->initialized) return;

    uint32_t width = ctx->width;
    uint32_t height = ctx->height;
    uint32_t cx = width / 2;
    uint32_t cy = height / 2;

    for (int frame = 0; frame < 360; frame += 2) {
        graphics_clear_screen(COLOR_BLACK);

        // draw multiple rotating rectangles
        for (int i = 0; i < 5; i++) {
            int32_t angle = frame + i * 72; // Degrees
            uint32_t size = 30 + i * 20;
            uint32_t distance = 50 + i * 30;
            // calculate position using integer math
            // math_cos/sin return values * 1000, so divide by 1000
            int32_t dx = (distance * math_cos(angle)) / 1000;
            int32_t dy = (distance * math_sin(angle)) / 1000;

            // calculate rectangle position, ensuring it stays in bounds
            int32_t rx = (int32_t) cx + dx;
            int32_t ry = (int32_t) cy + dy;

            // ensure rectangle stays on screen
            if (rx < (int32_t) (size / 2)) rx = size / 2;
            if (ry < (int32_t) (size / 2)) ry = size / 2;
            if (rx > (int32_t) (width - size / 2)) rx = width - size / 2;
            if (ry > (int32_t) (height - size / 2)) ry = height - size / 2;

            color_t colors[] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW, COLOR_CYAN};
            graphics_fill_rect((uint32_t) (rx - size / 2), (uint32_t) (ry - size / 2), size, size, colors[i]);
        }

        // draw center text
        graphics_draw_string(cx - 50, cy, "XG OS", COLOR_WHITE, COLOR_BLACK);

        graphics_delay(50000);
    }
}
