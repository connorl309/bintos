#pragma once

#define COM1 0x3f8
#define COM2 0x2f8

#include <stdint.h>
#include <stdbool.h>

// Serial functions
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ( "inb %w1, %b0"
                   : "=a"(ret)
                   : "Nd"(port)
                   : "memory");
    return ret;
}
bool serial_received(uint16_t port);
char read_serial(uint16_t port);
bool is_transmit_empty(uint16_t port);
void write_serial(uint16_t port, char a);
bool init_serial(uint16_t port);
void serial_log(uint16_t port, const char* str);