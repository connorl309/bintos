#include "stdlib.h"
void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    for (size_t i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }

    return dest;
}

void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;

    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }

    return s;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    if (src > dest) {
        for (size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if (src < dest) {
        for (size_t i = n; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
    }

    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

// Halt and catch fire function.
void hcf(void) {
    for (;;) {
        asm ("hlt");
    }
}

// Logging over serial
#include <stdarg.h>
#include "../serial/serial.h"

static const char* info = "[INFO]\t\t";
static const char* warn = "[WARN]\t\t";
static const char* error = "[ERROR]\t\t";
static const char* debug = "[DEBUG]\t\t";

// Assumes COM1 is already initialized with serial_init.
#if LOGGING_ENABLED
void logf(log_level l, const char* format, ...) {
    switch (l) {
        default:
            break;
        case INFO:
            serial_log(COM1, info);
            break;
        case WARN:
            serial_log(COM1, warn);
            break;
        case ERROR:
            serial_log(COM1, error);
            break;
        case DEBUG:
            serial_log(COM1, debug);
            break;
    }
    va_list args;
    va_start(args, format);

    char buffer[32]; // Temporary buffer for formatting numbers
    const char* str = format;

    while (*str) {
        if (*str == '%' && *(str + 1)) {
            ++str; // Move past '%'
            switch (*str) {
                case 's': {
                    const char* arg = va_arg(args, const char*);
                    if (arg) {
                        serial_log(COM1, arg);
                    }
                    break;
                }
                case 'd': {
                    int arg = va_arg(args, int);
                    bool is_negative = arg < 0;
                    if (is_negative) {
                        arg = -arg;
                        write_serial(COM1, '-');
                    }
                    int i = 0;
                    do {
                        buffer[i++] = '0' + (arg % 10);
                        arg /= 10;
                    } while (arg > 0);
                    for (int j = i - 1; j >= 0; --j) {
                        write_serial(COM1, buffer[j]);
                    }
                    break;
                }
                case 'x': {
                    unsigned int arg = va_arg(args, unsigned int);
                    int i = 0;
                    do {
                        int nibble = arg % 16;
                        buffer[i++] = nibble < 10 ? ('0' + nibble) : ('a' + (nibble - 10));
                        arg /= 16;
                    } while (arg > 0);
                    for (int j = i - 1; j >= 0; --j) {
                        write_serial(COM1, buffer[j]);
                    }
                    break;
                }
                default:
                    // Unknown specifier, just print it as is
                    write_serial(COM1, '%');
                    write_serial(COM1, *str);
                    break;
            }
        } else {
            write_serial(COM1, *str);
        }
        ++str;
    }

    va_end(args);
}

#else
void __attribute__((unused)) logf (log_level l, const char* format, ...) {}
#endif