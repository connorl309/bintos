#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void *memcpy(void *dest, const void *src, size_t n);

void *memset(void *s, uint8_t c, size_t n);

int memcmp(const void *s1, const void *s2, size_t n);
// Halt and catch fire function.
void hcf(void);
// Disable interrupts
void cli(void);

#define LOGGING_ENABLED 1 // Set this to 0 to disable logs

// Debug assumes that COM1 has been set up and initialzed properly.

typedef enum {
    INFO = 1,
    WARN = 2,
    ERROR = 3,
    DEBUG = 4,
    NUM_LEVELS
} log_level;

void logf(log_level l, const char* format, ...);