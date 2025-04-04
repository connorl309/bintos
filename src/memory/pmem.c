#include "../common.h"
#include "memory.h"

static struct limine_memmap_response* map;
static uint64_t bitmap_length;
static uint8_t* frame_map;
static uint64_t max_frames;
// Statistics
static uint64_t init_free_space;
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

// We have a page of memory reserved here for the structure information. This will be included in our 
// kernel reserved space so we don't really need to worry about it.
#define MAX_REGIONS 32
typedef struct {
    uint64_t start; // Starting address of the region
    uint64_t end; // Ending address of the region
    uint64_t pages; // # of pages the region can hold
} region_t;

static uint8_t num_regions;
region_t region_info[MAX_REGIONS];

// Initialize the kernel memory map
void initialize_memmap(struct limine_memmap_response *r, uint64_t offset) {
    hhdmoff = offset;
    map = r;
    CHASSERT(r->entry_count <= MAX_REGIONS && "Memory map has more regions than we can track!");
    uint8_t region_idx = 0;
    for (uint64_t i = 0; i < r->entry_count; i++) {
        struct limine_memmap_entry* e = r->entries[i];
        // Fancy print the type
        const char* type;
        switch(e->type) {
            // If usable we will add as region.
            case LIMINE_MEMMAP_USABLE:
                type = "Usable";
                init_free_space += e->length;
                region_info[region_idx++] = (region_t){e->base, e->base+e->length, e->length / FRAME_ALLOCATION_SIZE};
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

    num_regions = region_idx;

    // Parse and shove bitmap somewhere
    locate_for_bitmap();
    initialize_bitmap();
}
// Helper function to parse out the memory map into structures and values we
// need.
static void locate_for_bitmap() {
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
    bitmap_length = max_frames >> 1; // div by 2 (2 frames per byte). 
    // Don't care about odd/2 because it's fine if we're missing 1 page of memory to use.

    uint8_t usable_idx = 0;
    for (uint64_t i = 0; i < map->entry_count; i++) {
        const struct limine_memmap_entry* e = map->entries[i];

        // Find a USABLE block of memory that also
        // is big enough to put the bitmap.
        if (e->type == LIMINE_MEMMAP_USABLE) {
            if (e->length >= bitmap_length) {
                frame_map = (uint8_t*)e->base;
                // The memory region we install the bitmap onto must be
                // adjusted in the region information to account for the fact
                // that not all pages in the region are usable memory (due to the bitmap).
                // We need start to be page-aligned so guarantee that.
                region_info[usable_idx].start += bitmap_length;
                region_info[usable_idx].start = pg_round_up(region_info[usable_idx].start);
                region_info[usable_idx].pages = (region_info[usable_idx].end - region_info[usable_idx].start) / FRAME_ALLOCATION_SIZE;
            }
            usable_idx++;
        }
    }
    CHASSERT(frame_map && "Couldn't find a memmap region that had enough space for the frame bitmap!");
    frame_map += hhdmoff;
    logf(INFO, "Frame map beginning at 0x%lx, occupying %lx (%ld) bytes\n", frame_map, bitmap_length, bitmap_length);
}
// Initializes the bitmap for all frames
static void initialize_bitmap() {
    CHASSERT(frame_map && "Frame map wasn't initialized! Make sure locate_for_bitmap() is called!");

    // Since we only ever consider usable pages of memory when we initially construct the bitmap,
    // all pages should never be considered invalid.
    // Initially all pages will be available.
    const uint8_t set = (MAKE_FLAGS(AVAIL) << 4) + MAKE_FLAGS(AVAIL);
    for (uint32_t i = 0; i < bitmap_length; i++) {
        frame_map[i] = set;
    }
}
// Returns some physical frame.
void *frame_alloc() {
    NOT_IMPL();
    return NULL;
}
// Frees a frame.
void frame_free(void* paddr) {
    NOT_IMPL();
}