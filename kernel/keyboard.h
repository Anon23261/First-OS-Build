#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>
#include "interrupts.h"

// Buffer size
#define KEYBOARD_BUFFER_SIZE 256

// Special keys
#define KEY_ESC       0x01
#define KEY_BACKSPACE 0x0E
#define KEY_TAB       0x0F
#define KEY_ENTER     0x1C
#define KEY_CTRL      0x1D
#define KEY_LSHIFT    0x2A
#define KEY_RSHIFT    0x36
#define KEY_ALT       0x38
#define KEY_CAPS_LOCK 0x3A
#define KEY_F1        0x3B
#define KEY_F2        0x3C
#define KEY_HOME      0x47
#define KEY_UP        0x48
#define KEY_PGUP      0x49
#define KEY_LEFT      0x4B
#define KEY_RIGHT     0x4D
#define KEY_END       0x4F
#define KEY_DOWN      0x50
#define KEY_PGDN      0x51
#define KEY_DEL       0x53

#ifdef __cplusplus
extern "C" {
#endif

// Function declarations
void keyboard_init(void);
void keyboard_handler(struct registers* regs);
char keyboard_getchar(void);
bool keyboard_available(void);
void keyboard_clear_buffer(void);

#ifdef __cplusplus
}
#endif

#endif // KEYBOARD_H
