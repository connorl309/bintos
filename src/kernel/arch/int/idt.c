#include "idt.h"
#include "../../debug/debug.h"

__attribute__((aligned(sizeof(idt_entry_t))))
static idt_entry_t idt[256]; // The IDT can only hold up to 256 entries, this is hard-set by x86
static idtr_t idtr;
extern void* isr_stub_table[]; // from istub.asm

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

// Generic exception handler for exceptions
void exception_handler(intr_frame* frame) {
    __asm__ volatile ("cli");
    // There exist other exceptions past vector 15, but I don't really care about them (currently).
    if (frame->vec_no <= 20) {
        logf(ERROR, "CPU Exception[%d]: %s\n", frame->vec_no, exceptions[frame->vec_no]);
        logf(0, "Interrupt Frame:\n"
         "\tSegments:\n"
         "\t\tgs: 0x%x\tfs: 0x%x\tes: 0x%x\tds: 0x%x\n"
         "\tGeneral Purpose Registers (pusha):\n"
         "\t\tedi: 0x%x\tesi: 0x%x\tebp: 0x%x\tesp: 0x%x\n"
         "\t\tebx: 0x%x\tedx: 0x%x\tecx: 0x%x\teax: 0x%x\n"
         "\tInterrupt Info:\n"
         "\t\tvec_no: 0x%x\terr_code: 0x%x\n"
         "\tProcessor Pushed Info:\n"
         "\t\teip: 0x%x\tcs: 0x%x\teflags: 0x%x\n"
         "\t\tuser_esp: 0x%x\tss: 0x%x",
         frame->gs, frame->fs, frame->es, frame->ds,
         frame->edi, frame->esi, frame->ebp, frame->esp,
         frame->ebx, frame->edx, frame->ecx, frame->eax,
         frame->vec_no, frame->err_code,
         frame->eip, frame->cs, frame->eflags, 
         frame->user_esp, frame->ss);
    }
    __asm__ volatile ("hlt"); // Completely hangs the computer
    while (1);
}

// Sets IDT entry for `vector`.
void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    idt_entry_t* descriptor = &idt[vector];
    descriptor->isr_low        = (uintptr_t)isr & 0xFFFF;
    descriptor->kernel_cs      = 0x08; // must be kernel CS value from GDT (see helpers.asm)
    descriptor->attributes     = flags;
    descriptor->isr_high       = (uintptr_t)isr >> 16;
    descriptor->reserved       = 0;
}

// Generic init for the first 32 exceptions
void idt_register_exceptions() {
    idtr.base = (uintptr_t)idt;
    idtr.limit = (sizeof(idt_entry_t) * 256) - 1;
    
    for (uint8_t i = 0; i < 32; i++) {
        idt_set_descriptor(i, isr_stub_table[i], x86_INT);
    }

    __asm__ volatile ("lidt %0" : : "m"(idtr)); // just like lgdt
    __asm__ volatile ("sti"); // enable interrupts!!
}