#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include "interrupts.h"

// Function declarations
bool keyboard_initialize(void);
void keyboard_wait(void);
bool keyboard_ack(void);
bool keyboard_has_key(void);
uint8_t keyboard_read_scancode(void);
char keyboard_getchar(void);
void keyboard_handler(registers_t* regs);

#endif /* KEYBOARD_H */
