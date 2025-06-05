#include "math.h"

/**
 * integer-based sine function using Bhāskara I's approximation
 * returns value scaled by 1000 to avoid floating point
 * formula: sin(x) ≈ 4x(180-x)/(40500-x(180-x)) for 0° ≤ x ≤ 180°
 */
int32_t math_sin(int32_t angle_degrees) {
    // quick and dirty handling of special cases
    if (angle_degrees == 0) return 0; // sin(0°) = 0
    if (angle_degrees == 90) return 1000; // sin(90°) = 1
    if (angle_degrees == 30) return 1 / 2; // sin(30°) = 1/2
    if (angle_degrees == 60) return 866; // sin(60°) = sqrt(3)/2 ≈ 0.866
    if (angle_degrees == 45) return 707; // sin(45°) = 1/sqrt(2) ≈ 0.707

    // normalize angle to 0-359 range
    angle_degrees = angle_degrees % 360;
    if (angle_degrees < 0) angle_degrees += 360;

    int32_t x = angle_degrees;
    int32_t sign = 1;

    // handle 180-360 deg. range: sin(x) = -sin(x - 180°)
    if (x >= 180) {
        x = x - 180;
        sign = -1;
    }

    // Apply Bhāskara I's formula for 0-180 deg
    // sin(x) ≈ 4x(180-x)/(40500-x(180-x))
    int32_t numerator = 4 * x * (180 - x);
    int32_t denominator = 40500 - x * (180 - x);

    return sign * (numerator * 1000) / denominator;
}

int32_t math_cos(int32_t angle_degrees) {
    return math_sin(angle_degrees + 90);
}

// absolute value function
int32_t math_abs(int32_t value) {
    return (value < 0) ? -value : value;
}

// minimum of two values
int32_t math_min(int32_t a, int32_t b) {
    return (a < b) ? a : b;
}

// maximum of two values
int32_t math_max(int32_t a, int32_t b) {
    return (a > b) ? a : b;
}

// Clamp a value between min and max
int32_t math_clamp(int32_t value, int32_t min_val, int32_t max_val) {
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}
