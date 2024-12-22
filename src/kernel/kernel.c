#include <stdbool.h>
#include <stddef.h>
#include "arch/gdt.h"
#include "../debug/debug.h"
#include "../serial/serial.h"
#include "arch/multiboot.h"
#include "arch/int/idt.h"
#include "../lib/assert.h"
#include "../text/font.h"
#include "../lib/stdio.h"
#include "arch/int/interrupts.h"
#include "../devices/timer.h"

// GDT, other helpers
extern void gdt_flush();
extern void die();

// Passed in from GRUB
static struct multiboot_info* multiboot_info;

// Master kernel information - uptime
// We configure the PIT to tick every ~millisecond
uint32_t kernel_ticks = 0;

void kernel_main(struct multiboot_info* ptr, uint32_t multiboot_magic) 
{
	// Before we do *anything*, we need to grab the
	// Multiboot frame buffer pointer out of ebx.
	multiboot_info = ptr;

	assert(multiboot_magic == MULTIBOOT_BOOTLOADER_MAGIC);
	assert(multiboot_info);

	// GDT initialization
	bool s = true;
	s &= add_gdt_entry(0, 0, 0); // Null entry, required
	s &= add_gdt_entry(0, 0xFFFFF, (GDT_CODE_PL0)); // Kernel-code
	s &= add_gdt_entry(0, 0xFFFFF, (GDT_DATA_PL0)); // Kernel-data
	s &= add_gdt_entry(0, 0xFFFFF, (GDT_CODE_PL3)); // User-code
	s &= add_gdt_entry(0, 0xFFFFF, (GDT_DATA_PL3)); // User-data
	// TODO: Add a TSS section - can't just throw this wherever.
	assert(s);
	gdt_flush();
	if (init_serial(COM1)) {
		logf(INFO, "COM1 initialized!\n");
	}


	// Exceptions
	idt_register_exceptions();
	// Interrupts
	set_interrupt_routines();
	idt_register_interrupts(); // registers irq0-15 functions

	// Timer init
	pit_timer_init(1000); // 1000Hz (every millisecond)

	// Because we're booting into graphical mode from Multiboot, we don't have access to the VGA text buffer. As such
	// we have to actually draw the glyphs and characters from a font map.
	assert(multiboot_info->flags & MULTIBOOT_INFO_FRAMEBUFFER_INFO);
	init_font(multiboot_info->framebuffer_addr,
				  multiboot_info->framebuffer_pitch,
				  multiboot_info->framebuffer_width, multiboot_info->framebuffer_height,
				  0xFFFFFF, 0);

	
	// Kernel logging for some multiboot info: framebuffer information and memory map information
	logf(INFO, "Multiboot framebuffer located at 0x%x\n", (uintptr_t)multiboot_info->framebuffer_addr);
	logf(INFO, "Framebuffer Dimensions: %d x %d\n", multiboot_info->framebuffer_width, multiboot_info->framebuffer_height);

	// Memory map info
	assert(multiboot_info->flags & MULTIBOOT_INFO_MEM_MAP);
	multiboot_memory_map_t* mmap;
	logf(INFO, "mmap_addr = 0x%x, mmap_length = 0x%x\n", multiboot_info->mmap_addr, multiboot_info->mmap_length);
	for (mmap = (multiboot_memory_map_t *) multiboot_info->mmap_addr;
           (unsigned long) mmap < multiboot_info->mmap_addr + multiboot_info->mmap_length;
           mmap = (multiboot_memory_map_t *) ((unsigned long) mmap
                                    + mmap->size + sizeof (mmap->size)))
		{
			logf(INFO, "Base Physical Address = 0x%x%x,"
                " Region Length = 0x%x%x, %s\n",
                (unsigned) (mmap->addr >> 32),
                (unsigned) (mmap->addr & 0xFFFFFFFF),
                (unsigned) (mmap->len >> 32),
                (unsigned) (mmap->len & 0xFFFFFFFF),
                ((unsigned) mmap->type == 1) ? "Available" : "Reserved");
		}

	while (1); // (Eventually) we should never reach here
}
