#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "../debug/debug.h"
#include "../serial/serial.h"

#define PS2_DATA 0x60
#define PS2_STATUS 0x64
#define PS2_COMMAND 0x64

typedef enum {
    ERR_1 = 0x0, ERR_2 = 0xFF,
    SELF_TEST_PASS = 0xAA,
    ECHO_RESPONSE = 0xEE,
    ACK = 0xFA,
    SELF_TEST_FAIL_1 = 0xFC,
    SELF_TEST_FAIL_2 = 0xFD,
    RESEND = 0xFE,
    NUM_SPECIAL_KEY_INFO
} keys_special_info_e;

void keyboard_init();
void keyboard_handler();
const char* keyboard_read();