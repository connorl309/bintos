#include "keyboard_ps2.h"
#include "../lib/stdlib.h"

/** Bit 0 of the status register must be SET before attempting to read data from port 0x60
  * Bit 1 of the status register must be CLEAR before attempting to write data to port 0x60
*/
  #define WAIT_FOR_READ()   while ((inb(PS2_COMMAND) & 1u) == 0u) __asm__ volatile("rep nop");
  #define WAIT_FOR_WRITE()  while ((inb(PS2_COMMAND) & 2u) != 0u) __asm__ volatile("rep nop");


// Force-set scancode 2
void keyboard_init() {

}

/* The different modifier keys we support */
#define MOD_NONE  0
#define MOD_CTRL  (1 << 0)
#define MOD_SHIFT (1 << 1)
#define MOD_ALT   (1 << 2)

/* The modifier keys currently pressed */
static unsigned char mod_keys = 0;

/* A US QWERTY keymap, courtesy of Bran's tutorial */
unsigned char kbdmix[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '+', /*'´' */0, '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '<',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '-',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,  '<',
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

unsigned char kbdse_shift[128] = {
    0,  27, '!', '\"', '#', 0 /* shift+4 */, '%', '&', '/', '(',	/* 9 */
  ')', '=', '?', '`', '\b',	/* Backspace */
  '\t',			/* Tab */

 'Q', 'W', 'E', 'R',   /* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', 'A', 'A', '\n', /* Enter key */
    0,          /* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'O', /* 39 */
 '\'', '>',   0,        /* Left shift */
 '*', 'Z', 'X', 'C', 'V', 'B', 'N',            /* 49 */
  'M', ';', ':', '_',   0,              /* Right shift */

  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   '>',
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

#define KB_BUFFER_SZ 512
static char buffer[KB_BUFFER_SZ], reordered_buffer[KB_BUFFER_SZ];
static uint16_t buffer_head = 0, buffer_tail = 0;
static const char* empty = "\0";

#if KB_BUFFER_SZ > 512
#error "Keyboard buffer size must be <= 512 (keyboard_ps2.c)"
#endif

void keyboard_handler() {
    uint8_t val = inb(PS2_DATA);
    if (val == 0xE0) return; // don't care for this one (extended byte)

    // Check modifiers
    switch (val) {
		case 0x2a: /* shift down */
		case 0x36: { /* right shift down */
			mod_keys |= MOD_SHIFT;
			return;
			break;
        }
		case 0xaa: /* shift up */
		case 0xb6: { /* right shift up */
			mod_keys &= ~MOD_SHIFT;
			return;
			break;
        }

		case 0x1d: { /* ctrl down */
			mod_keys |= MOD_CTRL;
			return;
			break;
        }
		case 0x9d: { /* ctrl up */
			mod_keys &= ~MOD_CTRL;
			return;
			break;
        }

		case 0x38: { /* alt down */
			mod_keys |= MOD_ALT;
			return;
			break;
        }
		case 0xb8: { /* alt up */
			mod_keys &= ~MOD_ALT;
			return;
			break;
        }
		default:
			break;
	}

    // Need to implement keyboard hotkeys here, like terminal scrolling.
    char ascii = 0;
    if (mod_keys == MOD_NONE && !(val & 0x80)) {
		// No modifiers
		ascii = kbdmix[val];
	} else if (mod_keys == MOD_SHIFT && !(val & 0x80)) {
		// Shift + key
		ascii = kbdse_shift[val];
	} else if (mod_keys == MOD_CTRL && val == 0x20) {
		// Ctrl-D
		ascii = 4; // ASCII End of Transmission, good enough
	} else if (!(val & 0x80)) {
        logf(WARN, "Scancode 0x%x not supported\n", val);
    }
    if (!ascii) return;
    buffer[buffer_head] = ascii;
    buffer_head = (buffer_head + 1) % KB_BUFFER_SZ;
}

// Returns string from keyboard buffer
// Returns an empty string if there's nothing new in the buffer
const char* keyboard_read() {
    if (buffer_tail == buffer_head) return empty;

    memset(reordered_buffer, 0, KB_BUFFER_SZ);
    // Re-wrap the raw keyboard buffer into a string ptr we can return
    for (uint16_t i = 0; buffer_tail != buffer_head; i++) {
        reordered_buffer[i] = buffer[buffer_tail];
        buffer_tail = (buffer_tail + 1) % KB_BUFFER_SZ;
    }
    return reordered_buffer;
}
