#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "../serial/serial.h" // needed for outb

// Both units here are in hertz
#define PIT_MCLK_FREQ 1193180
#define PIT_CMD       0x43
#define PIT_CH0       0x40
// In QEMU we can't use CH2 (beeper), and don't care about CH1 (system-defined)

extern uint32_t kernel_ticks; 

// Initialize PIT for every `hertz` hertz
void pit_timer_init(uint16_t hertz);
// Generic timer delay function, in timer ticks
void timer_delay_ticks(uint32_t ticks);
// Generic timer delay function, in milliseconds
void timer_delay_ms(uint32_t ms);