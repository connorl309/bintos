#pragma once

#include <stdint.h>
#include <stdbool.h>

#define LOGGING_ENABLED 1 // Set this to 0 to disable logs

// Debug assumes that COM1 has been set up and initialzed properly.

typedef enum {
    INFO,
    WARN,
    ERROR,
    DEBUG,
    NUM_LEVELS
} log_level;

void log(log_level l, const char* msg);