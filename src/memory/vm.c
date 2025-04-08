#include "memory.h"

// Some defaults we will need
const static pml4e default_pml4e = {
    1, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0
};
const static pdpe default_pdpe = {
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0
};
const static pde default_pde = {
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0
};
const static pte default_pte = {
    0, 1, 0, 0, 0, 0,
    0, 0, 
    0, 1, 0, 0, 0, 0
};


// Linker variable once its done placing our binary
extern uint64_t KERNEL_END;

uint64_t pml4_addr;

// Set up PML4 and identity map the kernel and framebuffer.
void initialize_paging(struct limine_memmap_response* r, struct limine_file* kr, const struct limine_kernel_address_response* r2) {
    // We'll have to put PML4 in some dedicated physical address.
    // Initially zeroed-out.
    // PML4 itself has to be physically mapped as well.
    pml4_addr = frame_alloc(true);
    pml4e* table = (pml4e*)(pml4_addr + get_hhdmoff());
    map_page((uint64_t)table, pml4_addr, false, true, true, false);

    // We need the memmap for framebuffer (but can be refactored)
    for (uint8_t i = 0; i < r->entry_count; i++) {
        struct limine_memmap_entry* e = r->entries[i];
        // We will maintain the existing framebuffer mapping of hhdm + 0xFD...
        if (e->type == LIMINE_MEMMAP_FRAMEBUFFER) {
            for (uint64_t bytes = 0; bytes < e->length; bytes += FRAME_ALLOCATION_SIZE) {
                map_page(e->base + get_hhdmoff() + bytes, e->base + bytes, false, true, true, true);
            }
        }
    }

    // Map the kernel from both the file info (size) and the address response (addresses)
    uint64_t bytes = 0;
    for (uint64_t addr = KERNEL_VMA; addr <= (uint64_t)&KERNEL_END; addr += FRAME_ALLOCATION_SIZE) {
        map_page(addr, r2->physical_base + bytes, true, true, false, false);
        bytes += FRAME_ALLOCATION_SIZE;
    }

    uint64_t pml4o = PML4_INDEX(r2->virtual_base);
    uint64_t pdpo = PDPT_INDEX(r2->virtual_base);
    uint64_t pdo = PD_INDEX(r2->virtual_base);
    uint64_t pt = PT_INDEX(r2->virtual_base);

    asm volatile ("mov %0, %%cr3" : : "r" (pml4_addr) : "memory");
    logf(INFO, "Initialized kernel and framebuffer page tables. PML4 beginning at 0x%lx\n", pml4_addr);
}

// Maps a single page vaddr -> paddr
void map_page(uint64_t vaddr, uint64_t paddr, bool is_supervisor, bool writable, bool no_execute, bool writethru) {
    pml4e* pml4table = (pml4e*)(pml4_addr + get_hhdmoff());

    uint64_t pdpt;
    if (!pml4table[PML4_INDEX(vaddr)].present) {
        pdpt = frame_alloc(true);
        pml4table[PML4_INDEX(vaddr)].present = true;
        pml4table[PML4_INDEX(vaddr)].pdpe_ptr = pdpt;
        pdpt += get_hhdmoff();
    } else {
        pdpt = pml4table[PML4_INDEX(vaddr)].pdpe_ptr + get_hhdmoff();
    }

    pdpe* pdptable = (pdpe*)pdpt;
    uint64_t pd;
    if (!pdptable[PDPT_INDEX(vaddr)].present) {
        pd = frame_alloc(true);
        pdptable[PDPT_INDEX(vaddr)].present = true;
        pdptable[PDPT_INDEX(vaddr)].pde_ptr = pd;
        pd += get_hhdmoff();
    } else {
        pd = pdptable[PDPT_INDEX(vaddr)].pde_ptr + get_hhdmoff();
    }

    pde* pdetable = (pde*)pd;
    uint64_t pt;
    if (!pdetable[PD_INDEX(vaddr)].present) {
        pt = frame_alloc(true);
        pdetable[PD_INDEX(vaddr)].present = true;
        pdetable[PD_INDEX(vaddr)].pte_ptr = pt;
        pt += get_hhdmoff();
    } else {
        pt = pdetable[PD_INDEX(vaddr)].pte_ptr + get_hhdmoff();
    }

    pte* ptable = (pte*)pt;
    ptable[PT_INDEX(vaddr)].present = 1;
    ptable[PT_INDEX(vaddr)].rw = writable;
    ptable[PT_INDEX(vaddr)].us = is_supervisor;
    ptable[PT_INDEX(vaddr)].pwt = writethru;
    ptable[PT_INDEX(vaddr)].nx = !no_execute;
    ptable[PT_INDEX(vaddr)].frame = paddr >> 12;
}