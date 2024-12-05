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

// Key states
static bool keyboard_initialized = false;
static bool ctrl_pressed = false;
static bool alt_pressed = false;
static bool shift_pressed = false;
static bool caps_lock = false;

// Circular buffer for keyboard input
#define KEYBOARD_BUFFER_SIZE 128
static char key_buffer[KEYBOARD_BUFFER_SIZE];
static uint32_t buffer_start = 0;
static uint32_t buffer_end = 0;

// Keyboard maps
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

// Key codes
#define KEY_CTRL 0x1D
#define KEY_ALT 0x38
#define KEY_LSHIFT 0x2A
#define KEY_RSHIFT 0x36
#define KEY_CAPSLOCK 0x3A

bool keyboard_initialize() {
    // Wait for keyboard buffer to be empty
    keyboard_wait();
    
    // Reset keyboard LEDs
    outb(KEYBOARD_DATA_PORT, KEYBOARD_LED_CMD);
    if (!keyboard_ack()) return false;
    
    keyboard_wait();
    outb(KEYBOARD_DATA_PORT, 0x00);  // All LEDs off
    if (!keyboard_ack()) return false;
    
    // Clear the key buffer
    keyboard_buffer_clear();
    
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

// Buffer management functions
bool keyboard_buffer_is_empty() {
    return buffer_start == buffer_end;
}

bool keyboard_buffer_is_full() {
    return ((buffer_end + 1) % KEYBOARD_BUFFER_SIZE) == buffer_start;
}

void keyboard_buffer_put(char c) {
    if (!keyboard_buffer_is_full() && c != 0) {
        key_buffer[buffer_end] = c;
        buffer_end = (buffer_end + 1) % KEYBOARD_BUFFER_SIZE;
    }
}

char keyboard_buffer_get() {
    if (keyboard_buffer_is_empty()) {
        return 0;
    }
    char c = key_buffer[buffer_start];
    buffer_start = (buffer_start + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}

void keyboard_buffer_clear() {
    buffer_start = buffer_end = 0;
}

// Key state query functions
bool keyboard_is_shift_pressed() {
    return shift_pressed;
}

bool keyboard_is_ctrl_pressed() {
    return ctrl_pressed;
}

bool keyboard_is_alt_pressed() {
    return alt_pressed;
}

bool keyboard_is_caps_locked() {
    return caps_lock;
}

char keyboard_getchar() {
    return keyboard_buffer_get();
}

void keyboard_handler(registers_t* regs __attribute__((unused))) {
    if (!keyboard_has_key()) return;
    
    uint8_t scancode = keyboard_read_scancode();
    bool release = (scancode & 0x80) != 0;
    scancode &= 0x7F;  // Remove release bit
    
    // Handle special keys
    switch (scancode) {
        case KEY_CTRL:
            ctrl_pressed = !release;
            return;
        case KEY_ALT:
            alt_pressed = !release;
            return;
        case KEY_LSHIFT:
        case KEY_RSHIFT:
            shift_pressed = !release;
            return;
        case KEY_CAPSLOCK:
            if (!release) {  // Only toggle on key press, not release
                caps_lock = !caps_lock;
                // Update keyboard LEDs
                outb(KEYBOARD_DATA_PORT, KEYBOARD_LED_CMD);
                keyboard_wait();
                outb(KEYBOARD_DATA_PORT, caps_lock ? 4 : 0);
            }
            return;
    }
    
    // Only process key presses, not releases
    if (!release && scancode < sizeof(regular_map)) {
        bool uppercase = (shift_pressed != caps_lock);
        char c = uppercase ? shift_map[scancode] : regular_map[scancode];
        
        // Handle Ctrl combinations
        if (ctrl_pressed && c >= 'a' && c <= 'z') {
            c = c - 'a' + 1;  // Convert to control character (1-26)
        }
        
        keyboard_buffer_put(c);
    }
}
