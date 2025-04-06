#include "interrupts.h"

static idt_entry_t idt[INTR_CNT];
static idtr_t idtr;
extern void* exception_stub_table[32]; // from istubs
// More weird C code. Callbacks for all interrupts.
void (*handlers[INTR_CNT])(intr_frame* f) = { 0 };

#define PAGEFAULT 0xE
#define KERNEL_CS 0x28 // Looking at QEMU failure logs, we have x28?? Unsure.

// Quick helpers for enabling/disabling interrupts as needed
inline void intr_enable() { asm volatile ("sti"); }
inline void intr_disable() { asm volatile ("cli" : : : "memory"); }

// Helper function for IDT descriptors
static void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    idt_entry_t* descriptor = &idt[vector];

    descriptor->isr_low        = (uint64_t)isr & 0xFFFF;
    descriptor->kernel_cs      = KERNEL_CS;
    descriptor->ist            = 0;
    descriptor->attributes     = flags;
    descriptor->isr_mid        = ((uint64_t)isr >> 16) & 0xFFFF;
    descriptor->isr_high       = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    descriptor->reserved       = 0;
}

// Initialize IDT entries and the IDTR
// We also register exception handler stubs
void initialize_exceptions() {
    idtr.base = (uintptr_t)idt;
    idtr.limit = sizeof(idt_entry_t) * INTR_CNT - 1;
    // Exception handler callbacks
    for (uint8_t i = 0; i < 32; i++) {
        // Technically all of these redirect us into exception_handler but that's fine.
        idt_set_descriptor(i, exception_stub_table[i], 0x8E); // All of these are interrupt gates
    }

    asm volatile ("lidt %0" : : "m"(idtr));
    // intr_enable(); // We won't actually *enable* all interrupts here. The IDT is still valid
    // for exceptions without it.
    logf(INFO, "Registered exception handlers and loaded IDT!\n");
}

// General exception handler.
void exception_handler() {
    // For some reason, the compiler is fucking up the stack
    // here, and we can't directly use rdi. Annoying.
    // istubs.asm will shove the stack pointer into r15 before call.
    intr_frame* fr = NULL;
    asm volatile ("mov %%r15, %0" : "=r"(fr));

    void (*handler)(intr_frame* f) = NULL;
    bool external = fr->vec_no >= 0x20 && fr->vec_no < 0x30;

#ifdef MORE_DEBUG
    logf(DEBUG, "Exception handler called for code=0x%x [%s]\n", fr->vec_no, exceptions[fr->vec_no]);
#endif

    /* Invoke the interrupt's handler. */
    handler = handlers[fr->vec_no];
    if (handler != NULL) {
        handler(fr);
    } else if (fr->vec_no == 0x27 || fr->vec_no == 0x2f) {
        /* There is no handler, but this interrupt can trigger
         * spuriously due to a hardware fault or hardware race
         * condition.  Ignore it. */
    } else {
        logf(ERROR, "Received an interrupt/exception for something we have no handler for. Code = %x\n", fr->vec_no);
    }

    /* Complete the processing of an external interrupt. */
    if (external) {
        pic_eoi_ack(fr->vec_no);
    }

    hcf();
    while (1) { logf(ERROR, "Made it past HCF() in exception handler. You're fucked. F-U-C-K-E-D fucked.\n"); }
}

/* Sends an end-of-interrupt signal to the PIC for the given IRQ.
 * If we don't acknowledge the IRQ, it will never be delivered to
 * us again, so this is important.  */
void pic_eoi_ack(int irq)
{
    CHASSERT(irq >= 0x20 && irq < 0x30);

    /* Acknowledge master PIC. */
    outb(PIC0_CTRL, 0x20);

    /* Acknowledge slave PIC if this is a slave interrupt. */
    if (irq >= 0x28) {
        outb(PIC1_CTRL, 0x20);
    }
}