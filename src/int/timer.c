#include "timer.h"
#include "../common.h"

static uint16_t hz;
extern uint64_t ticks;
// Initialize PIT for every `hertz` hertz
void pit_timer_init(uint16_t hertz) {
    uint16_t divisor = PIT_MCLK_FREQ / hertz;
    outb(PIT_CMD, 0x36); // timer0, RW enabled LSB->MSB, square wave mode, no BCD
    outb(PIT_CH0, divisor & 0xFF);
    outb(PIT_CH0, divisor >> 8);
    logf(INFO, "Initialized PIT timer to tick @ %dHz\n", hertz);
    hz = hertz;
}
// Generic timer delay function, in timer ticks
// this is (obviously) blocking, and a bit wasteful...
void timer_delay_ticks(uint32_t t) {
    uint32_t expected = ticks + t;
    while (ticks < expected);
}
// Delay a specified amount of milliseconds
void timer_delay_ms(uint32_t ms) {
    uint32_t expected = ticks + ((ms * hz) / 1000);
    while (ticks < expected);
}