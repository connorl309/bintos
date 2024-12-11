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
    serial_log(COM1, "\n"); // manually append newline
}
#else
void __attribute__((unused)) log (log_level l, const char* msg) {}
#endif