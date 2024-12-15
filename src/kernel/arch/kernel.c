#include <stdbool.h>
#include <stddef.h>
#include "gdt.h"
#include "../debug/debug.h"
#include "../serial/serial.h"
#include "multiboot.h"
#include "../text/font.h"

// GDT, other helpers
extern void gdt_flush();
extern void die();

static volatile struct multiboot_info* multiboot_info;

void kernel_main(struct multiboot_info* multiboot_ptr) 
{
	// Before we do *anything*, we need to grab the
	// Multiboot frame buffer pointer out of ebx.
	multiboot_info = multiboot_ptr;

	// GDT initialization
	// TODO: should we do segmentation properly or just let memory
	// roam all over the place and deal with it in paging?
	bool s = true;
	s &= add_gdt_entry(0, 0, 0); // Null entry, required
	s &= add_gdt_entry(0, 0xFFFFF, (GDT_CODE_PL0)); // Kernel-code
	s &= add_gdt_entry(0, 0xFFFFF, (GDT_DATA_PL0)); // Kernel-data
	s &= add_gdt_entry(0, 0xFFFFF, (GDT_CODE_PL3)); // User-code
	s &= add_gdt_entry(0, 0xFFFFF, (GDT_DATA_PL3)); // User-data
	// TODO: Add a TSS section - can't just throw this wherever.
	if (s) {
		gdt_flush();
		if (init_serial(COM1)) {
			log(INFO, "COM1 initialized!\n");
		}
	} else {
		die();
	}

	// Because we're booting into graphical mode from Multiboot, we don't have access to the VGA text buffer. As such
	// we have to actually draw the glyphs and characters from a font map.
	// This is TODO.
	if (multiboot_info) {
		init_font(multiboot_info->framebuffer_addr,
				  multiboot_info->framebuffer_pitch,
				  multiboot_info->framebuffer_width, multiboot_info->framebuffer_height,
				  0xFFFFFF, 0);
		putchar('A');
		putchar('B');
	}
}
