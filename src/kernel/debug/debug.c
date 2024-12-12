#include "debug.h"
#include "../serial/serial.h"

static const char* info = "[INFO]\t\t";
static const char* warn = "[WARN]\t\t";
static const char* error = "!![ERROR]!!\t\t";
static const char* debug = "[DEBUG]\t\t";

// Assumes COM1 is already initialized with serial_init.
#if LOGGING_ENABLED
void log(log_level l, const char* msg) {
    switch (l) {
        default:
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
    serial_log(COM1, msg);
    serial_log(COM1, "\n");
}
// Logs a number (im not impl printf yet)
void log_num(uint64_t val) {
    const char hex_chars[] = "0123456789ABCDEF";
    int leading_zero = 1;
    serial_log(COM1, "0x");
    // Iterate over each 4-bit chunk, starting from the most significant nibble
    for (int i = 15; i >= 0; i--) {
        uint64_t nibble = (val >> (i * 4)) & 0xF;
        if (nibble != 0 || !leading_zero || i == 0) {
            leading_zero = 0; // Once a non-zero digit is printed, stop skipping
            write_serial(COM1, hex_chars[nibble]);
        }
    }
    write_serial(COM1, ' ');
}
#else
void __attribute__((unused)) log (log_level l, const char* msg) {}
void __attribute__((unused)) log_num(uint64_t val) {}
#endif