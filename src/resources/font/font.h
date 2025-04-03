#pragma once

#include <stdint.h>

/*
Linked font information:
Symbol table '.symtab' contains 4 entries:
   Num:    Value  Size Type    Bind   Vis      Ndx Name
     0: 00000000     0 NOTYPE  LOCAL  DEFAULT  UND
     1: 00000000     0 NOTYPE  GLOBAL DEFAULT    1 _binary_resources_Uni3_Terminus16_psf_start
     2: 00002a34     0 NOTYPE  GLOBAL DEFAULT    1 _binary_resources_Uni3_Terminus16_psf_end
     3: 00002a34     0 NOTYPE  GLOBAL DEFAULT  ABS _binary_resources_Uni3_Terminus16_psf_size
*/

extern char _binary_resources_Uni3_Terminus16_psf_start;
extern char _binary_resources_Uni3_Terminus16_psf_end;

void init_font(const uint32_t* addr, const uint32_t p, const uint32_t w, const uint32_t h, const uint32_t fg, const uint32_t bg);
void putchar(uint16_t c);
void set_text_cursor(uint32_t row, uint32_t col);
void puts(const char* str);