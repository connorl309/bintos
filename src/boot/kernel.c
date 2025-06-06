#include "../common.h"
#include "../serial/serial.h"
#include "../resources/font/font.h"
#include "../memory/memory.h"
#include "../int/interrupts.h"

#include "requests.h"

extern void* FRAME_START;
static uint64_t hhdm_offset;
uint64_t ticks = 0;

// The following will be our kernel's entry point.
// If renaming kmain() to something else, make sure to change the
// linker script accordingly.
void kmain(void) {
    uint64_t rsp;
    asm volatile ("mov %%rsp, %0" : "=r"(rsp));
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

    initialize_exceptions();

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

    // Now we need to begin mapping virtual memory for specific sections we care about.
    // This includes the framebuffer and the kernel regions. Anything else can be
    // mapped on the fly. Since we have control of VM,
    // we know for a fact what components of memory we need and
    // how the translation process works. The issue is when we want to handle
    // dynamic page faults we need to start allocating. 
    // We have the kernel space and framebuffer pre-reserved from the bootloader,
    // so no pallocs will need to happen there.
    initialize_paging(memmap_request.response, kernel, addr_request.response, rsp);
    // The kernel + framebuffer + stack are all mapped according to what Limine gave us,
    // i.e., the virtual addresses remain unchanged.

    // We are going to ignore interrupts for now. Teehee.
    // Device tree comes later as well.

    puts("Please work");
    // We're done, just hang...
    logf(WARN, "Reached end of kernel code. Hanging.\n");
    cli();
    hcf();
}