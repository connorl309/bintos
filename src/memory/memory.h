#pragma once

#include "../common.h"

// Change if we want? TBD. Must be dword aligned.
// Should probably align with the smallest page we make in VM.
#define FRAME_ALLOCATION_SIZE 4096

inline uint64_t v2p(uint64_t vaddr);
inline uint64_t p2v(uint64_t paddr);

/* Round up to nearest page boundary. */
static inline uint64_t pg_round_up (const uint64_t va) {
  return (((uint64_t) va + FRAME_ALLOCATION_SIZE - 1) & ~(FRAME_ALLOCATION_SIZE-1));
}

/* Round down to nearest page boundary. */
static inline uint64_t pg_round_down (const uint64_t va) {
  return ((uint64_t) va & ~(FRAME_ALLOCATION_SIZE-1));
}

// Initialize and read the kernel memory map
void        initialize_memmap(struct limine_memmap_response* r, uint64_t offset);

static void locate_for_bitmap();
// Initializes the bitmap by identifying where among the memmap we can put enough
// data to store the bitmap.
static void initialize_bitmap();
// Returns some physical frame.
void*       frame_alloc();
// Frees a frame.
void        frame_free(void* paddr);


/*&

315k chunk early on.

struct {
    ptr to region
    ptr to next region (starting AFTER bitmap)
    avail size = region size - bitmap size? pg round down
    bitmap size
} region



*/