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
    NOT_IMPL();
}