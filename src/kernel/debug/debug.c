#include "debug.h"
#include "../serial/serial.h"
#include <stdarg.h>

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