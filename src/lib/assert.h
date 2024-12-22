#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "../debug/debug.h"

extern void die();

#define assert(expr) \
    if (!(expr)) { \
        logf(ERROR, "Assertion failed @ %s:%d (%s), halting processor", __FILE__, __LINE__, __FUNCTION__); \
        die(); \
    }