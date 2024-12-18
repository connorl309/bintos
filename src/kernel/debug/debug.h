#pragma once

#include <stdint.h>
#include <stdbool.h>

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