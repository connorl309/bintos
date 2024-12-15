#include "font.h"
#include "../arch/multiboot.h"

static uint32_t* framebuffer_ptr;
static uint32_t pitch, width, height;
static uint16_t rows, cols, row_idx, col_idx;
static uint32_t foreground, background;

// https://www.zap.org.au/projects/console-fonts-distributed/psftx-debian-9.4/Uni3-Terminus16.psf.pdf
// Initialize font information (framebuffer ptr, screen information)
void init_font(const uint32_t addr, const uint32_t p, const uint32_t w, const uint32_t h, const uint32_t fg, const uint32_t bg) {
    framebuffer_ptr = (uint32_t*)addr;
    pitch = p/4; width = w; height = h;

    rows = h / 16; // font is 16 pixels high
    cols = w / 8; // font is 8 pixels wide
    row_idx = col_idx = 0; // initially we will presume drawing text at 0,0

    foreground = fg;
    background = bg;
}

// Write a single ascii character to the screen, location unspecified, and assumed to follow the `row_idx` and `col_idx` local
// variables.
void putchar(char c) {
    // Our starting index will be the value of `c` times 16.
    // Again, see https://www.zap.org.au/projects/console-fonts-distributed/psftx-debian-9.4/Uni3-Terminus16.psf.pdf
    // for a visual.

    // We want this to be optimized, hint to compiler to store this in register
    register uint32_t* ptr = framebuffer_ptr + (col_idx * 8) + (row_idx * pitch);

    uint16_t idx = c * 16;
    // do all 16 rows per glyph
    for (uint8_t i = 0; i < 16; i++) {
        // per byte, we need to figure out all bits that are painted/not (1/0).
        // do all 8 cols per row
        for (uint8_t j = 0; j < 8; j++) {
            if (uni3_terminus16_8_by_16_map[idx] & (1 << j)) {
                ptr[7-j] = foreground;
            } else {
                ptr[7-j] = background;
            }
        }
        ptr += pitch;
        idx++;
    }
    col_idx++;
    if (col_idx == cols) {
        col_idx = 0;
        row_idx++;
    }
}