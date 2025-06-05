#include <stdint.h>
#include "console.h"
#include "memory.h"
#include "paging.h"
#include "graphics.h"
#include "graphics_demo.h"

// early debug function to write directly to VGA memory
static void early_print(const char *msg) {
    static int pos = 0;
    volatile uint16_t *vga = (volatile uint16_t *) 0xB8000;
    while (*msg) {
        vga[pos++] = 0x0F00 | *msg; // white on black
        msg++;
    }
}

// Multiboot loader magic value passed in RAX
static const uint32_t MULTIBOOT_MAGIC = 0x2BADB002;

extern "C" void kernel_main(uint64_t magic, uint64_t mbi_addr) {
    early_print("64BIT START");

    if ((uint32_t) magic != MULTIBOOT_MAGIC) {
        early_print("BAD MAGIC");
        for (;;) {
        }
    }

    early_print("MAGIC OK");

    // memory management
    memory_init((uint32_t) mbi_addr);

    early_print("MEM OK");

    // set up advanced paging
    paging_init();

    early_print("PAGE OK");

    // try to initialize graphics subsystem
    if (framebuffer_detect((uint32_t) mbi_addr)) {
        early_print("FB OK");
        
        uint64_t fb_addr;
        uint32_t fb_width, fb_height, fb_pitch;
        uint8_t fb_bpp;
        
        framebuffer_get_info((uint32_t) mbi_addr, &fb_addr, &fb_width, 
                           &fb_height, &fb_pitch, &fb_bpp);
        
        early_print("FB INFO");
        
        // validate framebuffer parameters before proceeding
        if (fb_addr < 0x100000 || fb_width == 0 || fb_height == 0 || 
            fb_pitch == 0 || fb_bpp != 32) {
            early_print("FB BAD PARAMS");
        } else {
            uint64_t fb_size = (uint64_t)fb_pitch * fb_height;
            
            // map framebuffer to virtual memory
            if (paging_map_framebuffer(fb_addr, fb_size) == 0) {
                early_print("FB MAPPED");
                
                uint32_t *fb_ptr = (uint32_t *) fb_addr;
                
                if (graphics_init_simple(fb_ptr, fb_width, fb_height, fb_pitch, fb_bpp) == 0) {
                    early_print("GFX INIT OK");
                    
                    if (graphics_test_framebuffer() == 0) {
                        early_print("FB TEST OK");
                        
                        // clear screen and show initial message
                        graphics_clear_screen(COLOR_BLACK);
                        graphics_draw_string(10, 10, "XG OS Graphics Mode - Starting Animation...", COLOR_WHITE, COLOR_BLACK);
                        graphics_delay(1000000);
                        
                        // run bouncing ball animation
                        graphics_animate_bouncing_ball();
                        
                        // after animation, show final screen
                        graphics_clear_screen(COLOR_BLUE);
                        graphics_draw_rect(50, 50, 200, 100, COLOR_WHITE);
                        graphics_draw_string(60, 70, "XG OS Graphics Demo", COLOR_BLACK, COLOR_WHITE);
                        graphics_draw_string(60, 90, "Animation Complete!", COLOR_BLACK, COLOR_WHITE);
                        
                        early_print("ANIMATION DONE");
                        
                        // keep final screen visible
                        for (;;) {
                        }
                    } else {
                        early_print("FB TEST FAIL");
                    }
                } else {
                    early_print("GFX INIT FAIL");
                }
            } else {
                early_print("FB MAP FAIL");
            }
        }
    } else {
        early_print("NO FB");
    }

    clear();
    set_color(VGA_COLOR_RED);
    kprintf("Hello from XG OS 64-bit!\n");
    kprintf("Running in long mode\n");
    for (;;) {
    }
}
