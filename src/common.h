#pragma once

#include <stdint.h>
#include "lib/stdlib.h"
#include <stddef.h>
#include <stdarg.h>
#include "limine.h"
#include "lib/stdlib.h"

// Assert statement. Short for Cheddar Assert.
#define CHASSERT(expr) \
    if (!(expr)) { \
        logf(ERROR, "Assertion \'" #expr "\' failed %s:%d, halting processor", __FILE__, __LINE__); \
        hcf(); \
    }

#define NOT_IMPL()  \
    CHASSERT(false && "Not implemented!")