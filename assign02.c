#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Libraries for LED functionality 
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"



// Must declare the main assembly entry point before use.
void main_asm();

/*
 * Main entry point for the code - simply calls the main assembly function.
 */
int main() {
    main_asm();
    return(0);
}

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

//Usage:
//put_pixel(urgb_u32(R, G, B));