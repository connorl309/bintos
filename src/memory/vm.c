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
    for (uint16_t i = 0; i < FRAME_ALLOCATION_SIZE/sizeof(pml4e); i++) {
        pml4_structure[i] = default_pml4e;
        pml4_structure[i].pdpe_ptr = frame_alloc();
        pdpe* pdpe_structure = (pdpe*)(pml4_structure[i].pdpe_ptr + offs);
        for (uint16_t j = 0; j < FRAME_ALLOCATION_SIZE/sizeof(pdpe); j++) {
            pdpe_structure[j] = default_pdpe;
            pdpe_structure[j].pde_ptr = frame_alloc();
            pde* pde_structure = (pde*)(pdpe_structure[j].pde_ptr + offs);
            for (uint16_t k = 0; k < FRAME_ALLOCATION_SIZE/sizeof(pde); k++) {
                pde_structure[k] = default_pde;
                pde_structure[k].pte_ptr = frame_alloc();
                pte* pte_structure = (pte*)(pde_structure[k].pte_ptr + offs);
                for (uint16_t l = 0; l < FRAME_ALLOCATION_SIZE/sizeof(pte); l++) {
                    pte_structure[l] = default_pte;
                    // We divide because the PTE just holds the PFN, not the full address.
                    // TODO: Fix this to not use ALLOC, but actually just grab the necessary Limine memmap address.
                    // We don't want to "allocate", we just want to get the PA.
                    pte_structure[l].paddr = frame_alloc() / FRAME_ALLOCATION_SIZE;
                    // If the newly allocated page-frame is out of bounds of the current memory map region,
                    // go to the next one.
                    mmap_idx = mmap_idx + ((r->entries[mmap_idx]->base+r->entries[mmap_idx]->length) > (pte_structure[l].paddr << 12));
                    pte_structure[l].us = !(r->entries[mmap_idx]->type == LIMINE_MEMMAP_KERNEL_AND_MODULES);
                    pte_structure[l].rw = (r->entries[mmap_idx]->type == LIMINE_MEMMAP_RESERVED);
                    // Special care for any page that is part of the framebuffer.
                    // We want writeback cache for them.
                    // pwt = 0 means writeback, = 1 means writethrough
                    pte_structure[l].pwt = !(r->entries[mmap_idx]->type == LIMINE_MEMMAP_FRAMEBUFFER);
                }
            }
        }
    }
    // Revert the VM shift so we maintain the physical address.
    pml4_structure -= offs;
    logf(INFO, "Configured page tables, with PML4 beginning at physical 0x%lx.\n", pml4_structure);
    // Because we need Limine's paging configuration to be able to do any of this, we can't grab those memory regions.
    // It's only like 10MB of memory wasted so it's probably okay.
}