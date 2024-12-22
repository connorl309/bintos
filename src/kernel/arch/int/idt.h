#pragma once

#include <stdint.h>
#include <stdbool.h>

#define x86_INT (0x8E)
#define x86_TRAP (0x8F)

// IDT entry struct
typedef struct {
	uint16_t    isr_low;      // The lower 16 bits of the ISR's address
	uint16_t    kernel_cs;    // The GDT segment selector that the CPU will load into CS before calling the ISR
	uint8_t     reserved;     // Set to zero
	uint8_t     attributes;   // Type and attributes
	uint16_t    isr_high;     // The higher 16 bits of the ISR's address
} __attribute__((packed)) idt_entry_t;

// IDTR (just like GDTR)
typedef struct {
	uint16_t	limit;
	uint32_t	base;
} __attribute__((packed)) idtr_t;

// Interrupt frame structure
typedef struct {
	unsigned int gs, fs, es, ds;      /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned int vec_no, err_code;    /* our 'push byte #' and ecodes do this */
    unsigned int eip, cs, eflags, user_esp, ss;   /* pushed by the processor automatically */ 
} intr_frame;

// CPU exception handler. To implement later.
__attribute__((noreturn)) void exception_handler(intr_frame* f);

// Set IDT entry
void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags);
// Generic init for the first 32 exceptions
void idt_register_exceptions();
// Generic init for the first 16 interrupts
void idt_register_interrupts();