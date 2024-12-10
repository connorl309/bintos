#include <stdbool.h>
#include "gdt.h"

uint64_t gdt_table[GDT_ENTRIES];
struct gdt_ptr gdtr;

// Returns a GDT entry from `base`, `limit` and `flag`.
// set type, p, dpl, s, g, d/b, l and avl fields
static uint64_t create_descriptor(uint32_t base, uint32_t limit, uint16_t flag) {
    uint64_t descriptor;
 
    // Create the high 32 bit segment
    descriptor  =  limit       & 0x000F0000;         // set limit bits 19:16
    descriptor |= (flag <<  8) & 0x00F0FF00;
    descriptor |= (base >> 16) & 0x000000FF;         // set base bits 23:16
    descriptor |=  base        & 0xFF000000;         // set base bits 31:24
 
    // Shift by 32 to allow for low part of segment
    descriptor <<= 32;
 
    // Create the low 32 bit segment
    descriptor |= base  << 16;                       // set base bits 15:0
    descriptor |= limit  & 0x0000FFFF;               // set limit bits 15:0

    return descriptor;
}

// Adds an entry to the GDT, incrementally, each call
bool add_gdt_entry(uint32_t base, uint32_t limit, uint16_t flag) {
    // REQUIRED setup for lgdt instruction.
    gdtr.offset = (uint32_t)gdt_table;
    gdtr.size = (sizeof(uint64_t) * GDT_ENTRIES) - 1;

    static uint8_t index = 0;
    if (index < GDT_ENTRIES) {
        gdt_table[index++] = create_descriptor(base, limit, flag);
        return true;
    } else {
        return false;
    }
}