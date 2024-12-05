#include "keyboard.h"
#include "kernel.h"
#include <stdint.h>

// Keyboard ports
#define KEYBOARD_DATA_PORT    0x60
#define KEYBOARD_STATUS_PORT  0x64
#define KEYBOARD_COMMAND_PORT 0x64

// Keyboard commands
#define KEYBOARD_CMD_LED      0xED
#define KEYBOARD_CMD_ENABLE   0xF4
#define KEYBOARD_CMD_RESET    0xFF

// Buffer for keyboard input
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static int buffer_start = 0;
static int buffer_end = 0;
static bool shift_pressed = false;
static bool caps_lock = false;

// US keyboard layout
static const char scancode_to_ascii[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

static const char scancode_to_ascii_shift[] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' '
};

// Wait for keyboard controller to be ready
static void keyboard_wait_input() {
    while (inb(KEYBOARD_STATUS_PORT) & 2);
}

static void keyboard_wait_output() {
    while (!(inb(KEYBOARD_STATUS_PORT) & 1));
}

// Send command to keyboard controller
static void keyboard_send_cmd(uint8_t cmd) {
    keyboard_wait_input();
    outb(KEYBOARD_COMMAND_PORT, cmd);
}

// Send data to keyboard
static void keyboard_send_data(uint8_t data) {
    keyboard_wait_input();
    outb(KEYBOARD_DATA_PORT, data);
}

// Read keyboard response
static uint8_t keyboard_read_data() {
    keyboard_wait_output();
    return inb(KEYBOARD_DATA_PORT);
}

void keyboard_init() {
    // Reset keyboard
    keyboard_send_cmd(KEYBOARD_CMD_RESET);
    if (keyboard_read_data() != 0xFA) return;  // Acknowledge

    // Enable keyboard
    keyboard_send_cmd(KEYBOARD_CMD_ENABLE);
    if (keyboard_read_data() != 0xFA) return;  // Acknowledge

    buffer_start = 0;
    buffer_end = 0;
    shift_pressed = false;
    caps_lock = false;
}

extern "C" void keyboard_handler(struct registers* regs) {
    (void)regs;  // Unused parameter

    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    // Handle special keys
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = true;
        return;
    }
    if (scancode == (0x2A | 0x80) || scancode == (0x36 | 0x80)) {
        shift_pressed = false;
        return;
    }
    if (scancode == 0x3A) {
        caps_lock = !caps_lock;
        return;
    }

    // Only handle key press events (ignore key releases)
    if (scancode & 0x80) {
        return;
    }

    // Convert scancode to ASCII
    if (scancode < sizeof(scancode_to_ascii)) {
        char c;
        if (shift_pressed) {
            c = scancode_to_ascii_shift[scancode];
        } else {
            c = scancode_to_ascii[scancode];
        }

        // Apply caps lock
        if (caps_lock && ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) {
            c ^= 0x20;  // Toggle case
        }

        // Add to buffer if it's a printable character
        if (c) {
            int next_end = (buffer_end + 1) % KEYBOARD_BUFFER_SIZE;
            if (next_end != buffer_start) {  // Buffer not full
                keyboard_buffer[buffer_end] = c;
                buffer_end = next_end;
            }
        }
    }
}

extern "C" {
    char keyboard_getchar() {
        if (buffer_start == buffer_end) {
            return 0; // No character available
        }
        char c = keyboard_buffer[buffer_start];
        buffer_start = (buffer_start + 1) % KEYBOARD_BUFFER_SIZE;
        return c;
    }
}

bool keyboard_available() {
    return buffer_start != buffer_end;
}

void keyboard_clear_buffer() {
    buffer_start = buffer_end = 0;
}
