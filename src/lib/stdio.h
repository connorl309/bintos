#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "stdlib.h"
#include <stdarg.h>
#include "../text/font.h"

int printf(const char *format, ...);
int vprintf(const char *format, va_list arg);