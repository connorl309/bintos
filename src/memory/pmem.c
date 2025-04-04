#include "../common.h"
#include "memory.h"

static struct limine_memmap_response* map;
static uint8_t* frame_map;
static uint64_t max_frames;
// Statistics
static uint64_t init_free_space;
static uint64_t bytes_needed;
static uint64_t hhdmoff;

// Flags for a nibble
#define INVALID (1 << 0)
#define PINNED (1 << 1)
#define PRIVILEGED (1 << 2)
#define AVAIL (1 << 3)
#define MAKE_FLAGS(x) ((uint8_t)(x) & 0xF)

// TODO: Reevaluate this when paging is setup
inline uint64_t v2p(const uint64_t vaddr) {
    return vaddr - hhdmoff; 
}
inline uint64_t p2v(const uint64_t paddr) {
    return paddr + hhdmoff;
}

// Initialize the kernel memory map
void initialize_memmap(struct limine_memmap_response *r, uint64_t offset) {
    hhdmoff = offset;
    map = r;
    for (uint64_t i = 0; i < r->entry_count; i++) {
        struct limine_memmap_entry* e = r->entries[i];
        // Fancy print the type
        const char* type;
        switch(e->type) {
            case LIMINE_MEMMAP_USABLE:
                type = "Usable";
                init_free_space += e->length;
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
                init_free_space += e->length;
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
    max_frames = init_free_space / (FRAME_ALLOCATION_SIZE);
    logf(INFO, "%ld B\t/\t%d.%d GB\t/\t%ld frames of physical memory usable\n", init_free_space,
        ((init_free_space*100)/1073741824)/100, ((init_free_space*100)/1073741824)%100,
        max_frames);

    // Parse and shove bitmap somewhere
    parse_memmap();
    initialize_bitmap();
}
// Helper function to parse out the memory map into structures and values we
// need.
static void parse_memmap() {
    CHASSERT(map && "Memory map read failed!");
    /*
    Our bitmap will use some bit packing. We will use quartets of bits per-frame.    
    The quartet structure is as follows:
        xxxx    yyyy    zzzz
        ||||
        ||||_ invalid    - should we be interacting with this frame at all?
        |||__ pinned     - whether or not the frame can be evicted 
        ||___ privileged - whether or not kernel vs user owns this frame
        |____ free       - whether or not the frame is available for use
    */
    bytes_needed = (max_frames + 1) / 2; // ( +1)/2 to account for any odd-numbers of pages required.
    for (uint64_t i = 0; i < map->entry_count; i++) {
        const struct limine_memmap_entry* e = map->entries[i];
// TODO: need to also grab bootloader-reclaimable. Worry about it later I guess, lol.
        if (e->type == LIMINE_MEMMAP_USABLE && e->length >= bytes_needed) {
            frame_map = (uint8_t*)e->base;
            break;
        }
    }
    CHASSERT(frame_map && "Couldn't find a memmap region that had enough space for the frame bitmap!");
    frame_map += hhdmoff;
    logf(INFO, "Frame map beginning at 0x%lx, occupying %lx (%ld) bytes\n", frame_map, bytes_needed, bytes_needed);
}
// Initializes the bitmap by identifying where among the memmap we can put
// enough data to store the bitmap.
static void initialize_bitmap() {
    CHASSERT(frame_map && "Frame map wasn't initialized! Make sure initialize_memmap() is called!");

    memset(frame_map, 0, bytes_needed);

    // We need to set the frame info for all frames of physical memory we own
    uint64_t index = 0;
    uint64_t subindex = 0;

    for (uint64_t i = 0; i < map->entry_count; i++) {
        const struct limine_memmap_entry* e = map->entries[i];
        if (e->type == LIMINE_MEMMAP_USABLE) {
            // For all pages in this region...
            for (uint64_t j = e->base; j < e->base + e->length; j += FRAME_ALLOCATION_SIZE) {
                // Because we operate on nibbles we need to shift the flags
                // every odd page.
                frame_map[index] |= MAKE_FLAGS(AVAIL) << (subindex << 2);

                index = index + (subindex & 1);
                subindex = (subindex + 1) % 2; // roll-over counter for nibbles
            }
        } else if (e->type == LIMINE_MEMMAP_KERNEL_AND_MODULES) { // Special case kernel memory
            for (uint64_t j = e->base; j < e->base + e->length; j += FRAME_ALLOCATION_SIZE) {
                frame_map[index] |= MAKE_FLAGS(AVAIL | PINNED | PRIVILEGED) << (subindex << 2);
                index = index + (subindex & 1);
                subindex = (subindex + 1) % 2; // roll-over counter for nibbles
            }
        } else { // Otherwise don't care about the type, it's reserved and we shouldn't interact with it
            for (uint64_t j = e->base; j < e->base + e->length; j += FRAME_ALLOCATION_SIZE) {
                frame_map[index] |= MAKE_FLAGS(INVALID) << (subindex << 2);
                index = index + (subindex & 1);
                subindex = (subindex + 1) % 2; // roll-over counter for nibbles
            }
        }
    }
    logf(INFO, "Setup frame bitmap!\n");
}
// Returns some physical frame.
void *frame_alloc() {
    static uint64_t next_open_idx = 0;
    static uint64_t next_open_subidx = 0;

    uint64_t attempts = 0;
    
    uint8_t entry = 0;
    while (true) {
        entry = (frame_map[next_open_idx] >> (next_open_subidx << 2)) & 0xF;
        CHASSERT((attempts > 2*max_frames) && "Iterated over entire frame map twice and failed to find any available frame.");
        // Invalid frame?
        if (entry & INVALID) {
            next_open_idx = next_open_idx + (next_open_subidx & 1);
            next_open_subidx = (next_open_subidx + 1) % 2;
        }
        // Available?
        else if (entry & AVAIL) {
            next_open_idx = next_open_idx + (next_open_subidx & 1);
            next_open_subidx = (next_open_subidx + 1) % 2;
            attempts++;
            break;
        }
    }
}
// Frees a frame.
void frame_free(void* paddr) {

}