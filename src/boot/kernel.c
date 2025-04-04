#include "../common.h"
#include "../serial/serial.h"
#include "../resources/font/font.h"
#include "../memory/memory.h"

// Limine requests kept in this file
#include "requests.h"
// Please don't modify where it is relative to kmain : )

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

    init_font(framebuffer->address, framebuffer->pitch, framebuffer->width, framebuffer->height, 0x00FFFFFF, 0);

    initialize_memmap(memmap_request.response, hhdm_offset);

    puts("Please work");

    // Shouldn't get past this
    while (1) {}

    // We're done, just hang...
    cli();
    hcf();
}