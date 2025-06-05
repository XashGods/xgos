#ifndef GRAPHICS_DEMO_H
#define GRAPHICS_DEMO_H

#include "graphics.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// demo animations
void graphics_animate_bouncing_ball(void);

void graphics_animate_color_wave(void);

void graphics_animate_rotating_rects(void);

#ifdef __cplusplus
}
#endif

#endif // GRAPHICS_DEMO_H
