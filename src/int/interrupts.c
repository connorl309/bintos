#include "interrupts.h"

static idt_entry_t idt[INTR_CNT];
static idtr_t idtr;
extern void* exception_stub_table[32]; // from istubs
// More weird C code. Callbacks for all interrupts.
void (*handlers[INTR_CNT])(intr_frame* f) = { 0 };

#define PAGEFAULT 0xE



// General exception handler.
__attribute__((noreturn)) void exception_handler(intr_frame* fr) {
    asm volatile ("cli");

    void (*handler)(intr_frame* f) = NULL;
    bool external = fr->vec_no >= 0x20 && fr->vec_no < 0x30;

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

// PIC nonsense we have to deal with, otherwise as soon as we enable interrupts we'll fault
void pic_init(void) {
    /* Mask all interrupts on both PICs. */
    outb(PIC0_DATA, 0xff);
    outb(PIC1_DATA, 0xff);

    /* Initialize master. */
    outb(PIC0_CTRL, 0x11); /* ICW1: single mode, edge triggered, expect ICW4. */
    outb(PIC0_DATA, 0x20); /* ICW2: line IR0...7 -> irq 0x20...0x27. */
    outb(PIC0_DATA, 0x04); /* ICW3: slave PIC on line IR2. */
    outb(PIC0_DATA, 0x01); /* ICW4: 8086 mode, normal EOI, non-buffered. */

    /* Initialize slave. */
    outb(PIC1_CTRL, 0x11); /* ICW1: single mode, edge triggered, expect ICW4. */
    outb(PIC1_DATA, 0x28); /* ICW2: line IR0...7 -> irq 0x28...0x2f. */
    outb(PIC1_DATA, 0x02); /* ICW3: slave ID is 2. */
    outb(PIC1_DATA, 0x01); /* ICW4: 8086 mode, normal EOI, non-buffered. */

    /* Unmask all interrupts. */
    outb(PIC0_DATA, 0x00);
    outb(PIC1_DATA, 0x00);
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