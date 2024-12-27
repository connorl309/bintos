#include "interrupts.h"
#include "../../../serial/serial.h"

static void* irq_routines[16] = { 0 };

void* get_irq_handler(uint8_t num) {
    if (num <= 15)
        return irq_routines[num];
    else
        return 0;
}
void set_irq(uint8_t irq, void* isr) {
    irq_routines[irq] = isr;
}
void set_default_interrupt_routines() {
    irq_routines[0] = irq0;
    irq_routines[1] = irq1;
    irq_routines[2] = irq2;
    irq_routines[3] = irq3;
    irq_routines[4] = irq4;
    irq_routines[5] = irq5;
    irq_routines[6] = irq6;
    irq_routines[7] = irq7;
    irq_routines[8] = irq8;
    irq_routines[9] = irq9;
    irq_routines[10] = irq10;
    irq_routines[11] = irq11;
    irq_routines[12] = irq12;
    irq_routines[13] = irq13;
    irq_routines[14] = irq14;
    irq_routines[15] = irq15;
}

// System timer/clock interrupt - PIT
void irq0(intr_frame* f) {
extern uint32_t kernel_ticks;
    kernel_ticks++;
}

// Keyboard interrupt (supposedly)
void irq1(intr_frame* f) {
}

void irq2(intr_frame* f) {

}

void irq3(intr_frame* f) {

}

void irq4(intr_frame* f) {

}

void irq5(intr_frame* f) {

}

void irq6(intr_frame* f) {

}

void irq7(intr_frame* f) {

}

void irq8(intr_frame* f) {

}

void irq9(intr_frame* f) {

}

void irq10(intr_frame* f) {

}

void irq11(intr_frame* f) {

}

void irq12(intr_frame* f) {

}

void irq13(intr_frame* f) {

}

void irq14(intr_frame* f) {

}

void irq15(intr_frame* f) {

}

