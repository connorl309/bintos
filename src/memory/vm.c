#include "../common.h"
#include "memory.h"

// Some defaults we will need
const static pml4e default_pml4e = {
    1, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0
};
const static pdpe default_pdpe = {
    1, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0
};
const static pde default_pde = {
    1, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0
};
const static pte default_pte = {
    1, 1, 0, 0, 0, 1,
    0, 0, 
    0, 1, 0, 0, 0, 0
};


// Linker variable once its done placing our binary
extern void* KERNEL_END;


uint64_t pml4_addr;


// Set up PML4 and onwards.
// We will map all of physical memory.
void initialize_paging(struct limine_memmap_response* r) {
    const uint64_t offs = get_hhdmoff();
    pml4_addr = frame_alloc();

    pml4_addr += offs;
    uint8_t mmap_idx = 0;

    pml4e* pml4_structure = (pml4e*)pml4_addr;
    
    // Revert the VM shift so we maintain the physical address.
    pml4_structure -= offs;
    logf(INFO, "Configured page tables, with PML4 beginning at physical 0x%lx.\n", pml4_structure);
    // Because we need Limine's paging configuration to be able to do any of this, we can't grab those memory regions.
    // It's only like 10MB of memory wasted so it's probably okay.
    asm volatile ("movl %0, %%cr3" : : "r"(pml4_structure));
}