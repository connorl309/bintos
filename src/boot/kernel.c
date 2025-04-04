#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "limine.h"
#include "../lib/stdlib.h"


// Limine request list
__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);

// Framebuffer
__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

// HHDM for memory mapping later
__attribute__((used, section(".limine_requests")))
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

// Memory map
__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_kernel_file_request kernel_request = {
    .id = LIMINE_KERNEL_FILE_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

#include "../lib/stdlib.h"
#include "../serial/serial.h"
#include "../resources/font/font.h"

extern void* FRAME_START;
static uint64_t hhdm_offset;

// The following will be our kernel's entry point.
// If renaming kmain() to something else, make sure to change the
// linker script accordingly.
void kmain(void) {
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }

    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL
     || framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    // Initialize basic serial
    if (init_serial(COM1)) {
        logf(INFO, "Serial enabled on COM1\n");
    }

    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];
    struct limine_file* kernel = kernel_request.response->kernel_file;
    // log fb info
    logf(INFO, "Framebuffer identified at 0x%lx. Screen dimensions are %d x %d\n", framebuffer->address, framebuffer->width, framebuffer->height);

    // log kernel info
    logf(INFO, "Kernel '%s' from 0x%lx and 0x%lx\n", kernel->path, kernel->address, (uint64_t)kernel->address + kernel->size);

    // HHDM offset
    logf(INFO, "HHDM offset is 0x%lx\n", hhdm_request.response->offset);
    hhdm_offset = hhdm_request.response->offset;

    // Frame allocation location
    logf(INFO, "Frame allocation beginning at 0x%lx\n", &FRAME_START);

    // log memory map info
    for (uint64_t i = 0; i < memmap_request.response->entry_count; i++) {
        struct limine_memmap_entry* e = memmap_request.response->entries[i];
        // Fancy print the type
        const char* type;
        switch(e->type) {
            case LIMINE_MEMMAP_USABLE:
                type = "Usable";
                break;
            case LIMINE_MEMMAP_RESERVED:
                type = "Reserved";
                break;
            case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
                type = "ACPI Reclaimable";
                break;
            case LIMINE_MEMMAP_BAD_MEMORY:
                type = "Bad Memory";
                break;
            case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
                type = "Bootloader Reclaimable";
                break;
            case LIMINE_MEMMAP_KERNEL_AND_MODULES:
                type = "Kernel/Module";
                break;
            case LIMINE_MEMMAP_FRAMEBUFFER:
                type = "Framebuffer";
                break;
        }
        logf(INFO, "Memory Map: 0x%lx -> 0x%lx\t\t\t%s\n", e->base, e->base + e->length, type);
    }

    init_font(framebuffer->address, framebuffer->pitch, framebuffer->width, framebuffer->height, 0x00FFFFFF, 0);

    // Shouldn't get past this
    while (1) {}

    // We're done, just hang...
    hcf();
}