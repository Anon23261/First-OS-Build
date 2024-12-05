#include "keyboard.h"
#include "kernel.h"
#include <stdint.h>

// Keyboard ports
#define KEYBOARD_DATA_PORT    0x60
#define KEYBOARD_STATUS_PORT  0x64
#define KEYBOARD_COMMAND_PORT 0x64

// Keyboard status bits
#define KEYBOARD_OUTPUT_FULL  0x01
#define KEYBOARD_INPUT_FULL   0x02

// Keyboard commands
#define KEYBOARD_LED_CMD      0xED
#define KEYBOARD_ACK         0xFA

static bool keyboard_initialized = false;
static bool ctrl_pressed = false;
static bool shift_pressed = false;
static bool caps_lock = false;

bool keyboard_initialize() {
    // Wait for keyboard buffer to be empty
    keyboard_wait();
    
    // Reset keyboard LEDs
    outb(KEYBOARD_DATA_PORT, KEYBOARD_LED_CMD);
    keyboard_wait();
    outb(KEYBOARD_DATA_PORT, 0x00);  // All LEDs off
    
    keyboard_initialized = true;
    return true;
}

void keyboard_wait() {
    uint32_t timeout = 100000;
    while (timeout--) {
        if ((inb(KEYBOARD_STATUS_PORT) & KEYBOARD_INPUT_FULL) == 0) {
            return;
        }
    }
}

bool keyboard_ack() {
    uint32_t timeout = 100000;
    while (timeout--) {
        if (inb(KEYBOARD_STATUS_PORT) & KEYBOARD_OUTPUT_FULL) {
            if (inb(KEYBOARD_DATA_PORT) == KEYBOARD_ACK) {
                return true;
            }
        }
    }
    return false;
}

bool keyboard_has_key() {
    if (!keyboard_initialized) return false;
    return (inb(KEYBOARD_STATUS_PORT) & KEYBOARD_OUTPUT_FULL) != 0;
}

uint8_t keyboard_read_scancode() {
    while (!keyboard_has_key());
    return inb(KEYBOARD_DATA_PORT);
}

char keyboard_getchar() {
    if (!keyboard_has_key()) return 0;
    
    uint8_t scancode = keyboard_read_scancode();
    
    // Handle special keys
    switch (scancode) {
        case 0x1D: // Left CTRL press
            ctrl_pressed = true;
            return 0;
        case 0x9D: // Left CTRL release
            ctrl_pressed = false;
            return 0;
        case 0x2A: // Left SHIFT press
        case 0x36: // Right SHIFT press
            shift_pressed = true;
            return 0;
        case 0xAA: // Left SHIFT release
        case 0xB6: // Right SHIFT release
            shift_pressed = false;
            return 0;
        case 0x3A: // CAPS LOCK
            caps_lock = !caps_lock;
            return 0;
    }
    
    // Handle regular keys
    if (scancode & 0x80) {
        // Key release
        return 0;
    }
    
    // Convert scancode to ASCII
    char c = 0;
    if (scancode < 128) {
        static const char regular_map[] = {
            0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
            '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
            0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
            0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
            '*', 0, ' '
        };
        
        static const char shift_map[] = {
            0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
            '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
            0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
            0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
            '*', 0, ' '
        };
        
        if (scancode < sizeof(regular_map)) {
            bool uppercase = (shift_pressed != caps_lock);
            c = uppercase ? shift_map[scancode] : regular_map[scancode];
            
            // Handle Ctrl combinations
            if (ctrl_pressed && c >= 'a' && c <= 'z') {
                c = c - 'a' + 1;  // Convert to control character (1-26)
            }
        }
    }
    
    return c;
}

void keyboard_handler(registers_t* regs __attribute__((unused))) {
    if (keyboard_has_key()) {
        keyboard_getchar();  // Process the key
    }
}
