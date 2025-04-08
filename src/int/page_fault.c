#include "interrupts.h"
#include "../memory/memory.h"

// We get called from exception_handler.
void page_fault(intr_frame* fr) {
    uint64_t faulting_addr = read_cr2();
    logf(DEBUG, "Page fault on 0x%lx\n", faulting_addr);
}