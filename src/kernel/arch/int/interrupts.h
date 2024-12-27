#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "../../../debug/debug.h"
#include "idt.h"

void set_default_interrupt_routines();
void set_irq(uint8_t irq, void* isr);
void* get_irq_handler(uint8_t num);
void irq0(intr_frame* f);
void irq1(intr_frame* f);
void irq2(intr_frame* f);
void irq3(intr_frame* f);
void irq4(intr_frame* f);
void irq5(intr_frame* f);
void irq6(intr_frame* f);
void irq7(intr_frame* f);
void irq8(intr_frame* f);
void irq9(intr_frame* f);
void irq10(intr_frame* f);
void irq11(intr_frame* f);
void irq12(intr_frame* f);
void irq13(intr_frame* f);
void irq14(intr_frame* f);
void irq15(intr_frame* f);
