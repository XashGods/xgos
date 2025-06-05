#ifndef MATH_H
#define MATH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// integer-based trigonometric functions for kernel use
// these functions return values scaled by 1000 to avoid floating point
// for example, sin(90 deg) returns 1000 instead of 1.0

/**
 * Integer-based sine function
 * @param angle_degrees Angle in degrees (0-359)
 * @return Sine value scaled by 1000 (-1000 to +1000)
 */
int32_t math_sin(int32_t angle_degrees);

/**
 * Integer-based cosine function  
 * @param angle_degrees Angle in degrees (0-359)
 * @return Cosine value scaled by 1000 (-1000 to +1000)
 */
int32_t math_cos(int32_t angle_degrees);

/**
 * Absolute value function
 * @param value Input value
 * @return Absolute value of input
 */
int32_t math_abs(int32_t value);

/**
 * Minimum of two values
 * @param a First value
 * @param b Second value
 * @return Smaller of the two values
 */
int32_t math_min(int32_t a, int32_t b);

/**
 * Maximum of two values
 * @param a First value
 * @param b Second value
 * @return Larger of the two values
 */
int32_t math_max(int32_t a, int32_t b);

/**
 * Clamp a value between min and max
 * @param value Value to clamp
 * @param min_val Minimum allowed value
 * @return Clamped value
 */
int32_t math_clamp(int32_t value, int32_t min_val, int32_t max_val);

#ifdef __cplusplus
}
#endif

#endif // MATH_H
