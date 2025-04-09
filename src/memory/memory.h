#pragma once

#include "../common.h"

// Change if we want? TBD. Must be dword aligned.
// Should probably align with the smallest page we make in VM.
#define FRAME_ALLOCATION_SIZE 4096
#define KERNEL_VMA 0xffffffff80000000


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
uint64_t       frame_alloc(bool zeroed);
// Frees a frame.
void        frame_free(void* paddr);
// Returns total number of physical frames
uint64_t get_max_frames();
uint64_t get_hhdmoff();

// https://www.amd.com/content/dam/amd/en/documents/processor-tech-docs/programmer-references/40332.pdf
// Volume 3.5 / pg 583

/*

  vm info

*/
// Helper macros for extracting indices from a virtual address
#define PML4_INDEX(vaddr) (((vaddr) >> 39) & 0x1FF)
#define PDPT_INDEX(vaddr) (((vaddr) >> 30) & 0x1FF)
#define PD_INDEX(vaddr)   (((vaddr) >> 21) & 0x1FF)
#define PT_INDEX(vaddr)   (((vaddr) >> 12) & 0x1FF)

void map_page(uint64_t vaddr, uint64_t paddr, bool is_supervisor, bool writable, bool no_execute, bool writethru);

// We should probably verify this works.
static inline uint64_t read_cr3() {
    uint64_t v;
    // inline asm: =r means destination, r means src.
    asm volatile ("mov cr3, %0" : "=r"(v));
    return v;
}
// This function is not friendly.
// The address MUST be 4k aligned.
static inline void set_cr3(uint64_t newValue) {
    CHASSERT(pg_round_down(newValue) == newValue && "Value to set in CR3 is not page-aligned!");
    asm volatile ("mov %0, %%cr3"
    :
    : "r" (newValue)
    : "memory"
);
}

typedef struct __attribute__((packed, aligned(8))) {
    uint64_t present : 1;
    uint64_t rw : 1;
    uint64_t us : 1;
    uint64_t pwt : 1;
    uint64_t pcd : 1;
    uint64_t accessed : 1;
    uint64_t ign1 : 1;
    uint64_t zero1 : 1;
    uint64_t zero2 : 1;
    uint64_t dummy : 3; // useless
    uint64_t pdpe_ptr : 40;
    uint64_t dummy2 : 11;
    uint64_t nx : 1;
} pml4e;

typedef struct __attribute__((packed, aligned(8))) {
    uint64_t present : 1;
    uint64_t rw : 1;
    uint64_t us : 1;
    uint64_t pwt : 1;
    uint64_t pcd : 1;
    uint64_t accessed : 1;
    uint64_t ign1 : 1;
    uint64_t zero : 1;
    uint64_t ign2 : 1;
    uint64_t dummy : 3; // useless
    uint64_t pde_ptr : 40;
    uint64_t dummy2 : 11;
    uint64_t nx : 1;
} pdpe;

typedef struct __attribute__((packed, aligned(8))) {
    uint64_t present : 1;
    uint64_t rw : 1;
    uint64_t us : 1;
    uint64_t pwt : 1;
    uint64_t pcd : 1;
    uint64_t accessed : 1;
    uint64_t ign1 : 1;
    uint64_t zero : 1;
    uint64_t ign2 : 1;
    uint64_t dummy : 3; // useless
    uint64_t pte_ptr : 40;
    uint64_t dummy2 : 11;
    uint64_t nx : 1;
} pde;

typedef struct __attribute__((packed, aligned(8))) {
    uint64_t present : 1;
    uint64_t rw : 1;
    uint64_t us : 1;
    uint64_t pwt : 1;
    uint64_t pcd : 1;
    uint64_t accessed : 1;
    uint64_t dirty : 1;
    uint64_t pat : 1;
    uint64_t g : 1;
    uint64_t avail : 3; // useless
    uint64_t frame : 40;
    uint64_t dummy : 7; // useless
    uint64_t dummy2 : 4;
    uint64_t nx : 1;
} pte;

// Reads the cr2 register.
static inline uint64_t read_cr2() {
    uint64_t cr2;
    asm ("mov %%cr2, %0" : "=r"(cr2));
    return cr2;
}

void initialize_paging(struct limine_memmap_response* r, struct limine_file* kr, const struct limine_kernel_address_response* r2, uint64_t rsp);