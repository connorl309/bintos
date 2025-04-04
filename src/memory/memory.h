#pragma once

#include "../common.h"

// Change if we want? TBD. Must be dword aligned.
// Should probably align with the smallest page we make in VM.
#define FRAME_ALLOCATION_SIZE 4096

inline uint64_t v2p(uint64_t vaddr);
inline uint64_t p2v(uint64_t paddr);

// Initialize the kernel memory map
void        initialize_memmap(struct limine_memmap_response* r, uint64_t offset);
// Helper function to parse out the memory map into structures and values we need.
static void parse_memmap();
// Initializes the bitmap by identifying where among the memmap we can put enough
// data to store the bitmap.
static void initialize_bitmap();
// Returns some physical frame.
void*       frame_alloc();
// Frees a frame.
void        frame_free(void* paddr);