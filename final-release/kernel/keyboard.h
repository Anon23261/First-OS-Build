#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include "interrupts.h"

// Key buffer size
#define KEYBOARD_BUFFER_SIZE 256

// Special key codes
#define KEY_ESCAPE    0x01
#define KEY_BACKSPACE 0x0E
#define KEY_TAB       0x0F
#define KEY_ENTER     0x1C
#define KEY_CTRL      0x1D
#define KEY_LSHIFT    0x2A
#define KEY_RSHIFT    0x36
#define KEY_ALT       0x38
#define KEY_CAPSLOCK  0x3A
#define KEY_F1        0x3B
#define KEY_F2        0x3C
#define KEY_F3        0x3D
#define KEY_F4        0x3E
#define KEY_F5        0x3F
#define KEY_F6        0x40
#define KEY_F7        0x41
#define KEY_F8        0x42
#define KEY_F9        0x43
#define KEY_F10       0x44
#define KEY_F11       0x57
#define KEY_F12       0x58

// Function declarations
bool keyboard_initialize(void);
void keyboard_wait(void);
bool keyboard_ack(void);
bool keyboard_has_key(void);
uint8_t keyboard_read_scancode(void);
char keyboard_getchar(void);
void keyboard_handler(registers_t* regs);

// Buffer management
bool keyboard_buffer_is_empty(void);
bool keyboard_buffer_is_full(void);
void keyboard_buffer_put(char c);
char keyboard_buffer_get(void);
void keyboard_buffer_clear(void);

// Key state queries
bool keyboard_is_shift_pressed(void);
bool keyboard_is_ctrl_pressed(void);
bool keyboard_is_alt_pressed(void);
bool keyboard_is_caps_locked(void);

#endif /* KEYBOARD_H */
