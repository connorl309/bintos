#include "stdio.h"

// Bintos' printf() only supports %s, %d and %x for now.
int printf(const char *format, ...) {
    va_list args;
    int done;

    va_start(args, format);
    done = vprintf(format, args);
    va_end(args);
    return done;
}

// Supporting %s, %d, %x for now
int vprintf(const char *format, va_list args) {
    int printed = 0;
    uint16_t index = 0;

    const char* current = &format[index];
    char scratch[11] = {0};
    uint8_t scratch_idx = 9;

    // Don't want to print past end of str
    while (*(current = &format[index])) {
        char val = *current;
        // Check 1 - basic character
        if (val != '%') {
            putchar(val);
        }
        else {
            // Format specifier follows. It can be either s, d, or x (for now).
            current++;
            val = *current;
            // Parse the specifier
            switch (val) {
                default:
                    puts("Invalid format specifier!");
                    return -1;
                    break;
                case 's': {
                    const char* string = va_arg(args, const char*);
                    puts(string);
                    printed += strlen(string);
                    break;
                }
                case 'd': {
                    int value = va_arg(args, int);
                    int sum = value;
                    if (value < 0) {
                        putchar('-');
                        printed++;
                        value *= -1;
                    }
                    do {
                        int digit = sum % 10;
                        scratch[scratch_idx--] = digit + '0';
                        sum /= 10;
                        printed++;
                    } while (sum);
                    scratch[scratch_idx] = 0;
                    puts(scratch + scratch_idx + 1);
                    break;
                }
                case 'x': {
                    int value = va_arg(args, int);
                    uint32_t sum = (uint32_t)value;
                    do {
                        int digit = sum % 16;
                        if (digit < 10)
                            scratch[scratch_idx--] = digit + '0';
                        else
                            scratch[scratch_idx--] = (digit - 0xA) + 'a';
                        sum /= 16;
                        printed++;
                    } while (sum);
                    puts(scratch + scratch_idx + 1);
                    break;
                }
            }
            index++;
            scratch_idx = 9; // Reset scratch space for 32 bit numbers
        }
        index++;
        printed++;
    }

    return printed;
}