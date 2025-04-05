#pragma once

#include "../common.h"
#include "../serial/serial.h"

#define PIC0_DATA 0x21
#define PIC0_CTRL 0x20
#define PIC1_DATA 0xA1
#define PIC1_CTRL 0xA0

// Basic messages for which exception happened
static const char* exceptions[20] = {
    "#DE - Division Error",
    "#DB - Debug",
    "NMI",
    "#BP - Breakpoint",
    "#OF - Overflow",
    "#BR - Bound Range Exceeded",
    "#UD - Invalid Opcode",
    "#NM - Device Not Available",
    "#DF - Double Fault",
    "N/A",
    "#TS - Invalid TSS",
    "#NP - Segment Not Present",
    "#SS - Stack Segment Fault",
    "#GP - General Protection Fault",
    "#PF - Page Fault",
    "RESERVED",
    "#MF - x87 Floating-Point Exception",
    "#AC - Alignment Check",
    "#MC - Machine Check",
    "#XM/#XF - SIMD Floating-Point Exception"
};

typedef struct {
	uint16_t    isr_low;      // The lower 16 bits of the ISR's address
	uint16_t    kernel_cs;    // The GDT segment selector that the CPU will load into CS before calling the ISR
	uint8_t	    ist;          // The IST in the TSS that the CPU will load into RSP; set to zero for now
	uint8_t     attributes;   // Type and attributes; see the IDT page
	uint16_t    isr_mid;      // The higher 16 bits of the lower 32 bits of the ISR's address
	uint32_t    isr_high;     // The higher 32 bits of the ISR's address
	uint32_t    reserved;     // Set to zero
} __attribute__((packed)) idt_entry_t;

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) idtr_t;

/* Interrupt stack frame. */
typedef struct {
	//uint64_t gs, fs, es, ds;      /* pushed the segs last */
    uint64_t rdi, rsi, rbp, dummy, rbx, rdx, rcx, rax;  /* pushed by 'pusha' */
    uint64_t vec_no, err_code;    /* our 'push byte #' and ecodes do this */
    uint64_t rip, cs, eflags, rsp, ss;   /* pushed by the processor automatically */ 
} intr_frame;

__attribute__((noreturn)) void exception_handler(intr_frame* fr);

#define INTR_CNT 256


// Enable interrupts
void intr_enable();
// Disable interrupts
void intr_disable();

// PIC specifics
void pic_init();
void pic_remap();
void pic_eoi_ack(int irq);