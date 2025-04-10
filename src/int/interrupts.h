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
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rdi, rsi, rbp, rdx, rcx, rbx, rax;  /* pushed by us */
    uint64_t vec_no, err_code;    /* our 'push byte #' and ecodes do this */
    uint64_t rip, cs, eflags, rsp, ss;   /* pushed by the processor automatically */ 
} intr_frame;

__attribute__((noreturn)) void exception_handler();

#define INTR_CNT 256
#define NUM_EXCEPTIONS 32


// Enable interrupts
void intr_enable();
// Disable interrupts
void intr_disable();
void initialize_exceptions();

void page_fault(intr_frame* fr);

// PIC specifics
// void pic_init();
// void pic_remap();
void pic_eoi_ack(int irq);

/*

Remember, we remap all IRQs to come after the exceptions, so
starting at entry #32.

MASTER:

IVT Offset | INT # | IRQ # | Description
-----------+-------+-------+------------------------------
0x0020     | 0x08  | 0     | PIT
0x0024     | 0x09  | 1     | Keyboard
0x0028     | 0x0A  | 2     | 8259A slave controller
0x002C     | 0x0B  | 3     | COM2 / COM4
0x0030     | 0x0C  | 4     | COM1 / COM3
0x0034     | 0x0D  | 5     | LPT2
0x0038     | 0x0E  | 6     | Floppy controller
0x003C     | 0x0F  | 7     | LPT1

SLAVE:

IVT Offset | INT # | IRQ # | Description
-----------+-------+-------+------------------------------
0x01C0     | 0x70  | 8     | RTC
0x01C4     | 0x71  | 9     | Unassigned
0x01C8     | 0x72  | 10    | Unassigned
0x01CC     | 0x73  | 11    | Unassigned
0x01D0     | 0x74  | 12    | Mouse controller
0x01D4     | 0x75  | 13    | Math coprocessor
0x01D8     | 0x76  | 14    | Hard disk controller 1
0x01DC     | 0x77  | 15    | Hard disk controller 2

*/

// void assign_interrupt_vectors();