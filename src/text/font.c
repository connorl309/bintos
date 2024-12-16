#include "font.h"
#include "psf.h"

static uint32_t* framebuffer_ptr;
static uint32_t pitch, width, height;
static uint16_t rows, cols, row_idx, col_idx;
static uint32_t foreground, background;
static uint8_t* font_data;

// https://www.zap.org.au/projects/console-fonts-distributed/psftx-debian-9.4/Uni3-Terminus16.psf.pdf
// Initialize font information (framebuffer ptr, screen information)
void init_font(const uint32_t addr, const uint32_t p, const uint32_t w, const uint32_t h, const uint32_t fg, const uint32_t bg) {
    framebuffer_ptr = (uint32_t*)addr;
    pitch = p/sizeof(uint32_t); width = w; height = h;

    rows = h / 16; // font is 16 pixels high
    cols = w / 8; // font is 8 pixels wide
    row_idx = col_idx = 0; // initially we will presume drawing text at 0,0

    foreground = fg;
    background = bg;

    // Our font is a PSF1 so there's only 4 total bytes of information as the "header".
    // TODO: make this work with either PSF1 or PSF2
    PSF1_Font* f = (PSF1_Font*)&_binary_resources_Uni3_Terminus16_psf_start;
    font_data = (uint8_t*)(++f);
}

// Sets the text cursor for printing to the screen
void set_text_cursor(uint32_t row, uint32_t col) {
    row_idx = row;
    col_idx = col;
}

// Write a single ascii character to the screen, location unspecified, and assumed to follow the `row_idx` and `col_idx` local
// variables.
void putchar(uint16_t c) {
    // Our starting index will be the value of `c` times 16.
    // Again, see https://www.zap.org.au/projects/console-fonts-distributed/psftx-debian-9.4/Uni3-Terminus16.psf.pdf
    // for a visual.
    if (c == '\n') {
        row_idx++;
        col_idx = 0;
        return;
    }
    if (c == '\t') {
        col_idx += 4;
        return;
    }

    // We want this to be optimized, hint to compiler to store this in register
    register uint32_t* ptr = framebuffer_ptr + (col_idx * 8) + (row_idx * pitch * 16);

    uint16_t idx = c * 16;
    // do all 16 rows per glyph
    for (uint8_t i = 0; i < 16; i++) {
        // per byte, we need to figure out all bits that are painted/not (1/0).
        // do all 8 cols per row
        for (uint8_t j = 0; j < 8; j++) {
            if (font_data[idx] & (1 << j)) {
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

// Write a single string, unformatted.
void puts(const char* str) {
    while (*str) {
        putchar(*str);
        str++;
    }
}