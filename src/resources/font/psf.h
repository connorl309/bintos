#pragma once

#include <stdint.h>

#define PSF1_FONT_MAGIC 0x3604
#define PSF2_FONT_MAGIC 0x864ab572

typedef struct {
    uint16_t magic; // Should be == `PSF1_FONT_MAGIC`
    uint8_t mode;
    uint8_t glyph_size;
} PSF1_Font;

typedef struct {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
} PSF2_Font;