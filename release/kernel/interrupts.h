#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>
#include <stdbool.h>

// Interrupt numbers
#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

// Register structure passed to handlers
struct registers {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
};
typedef struct registers registers_t;

// Function pointer type for interrupt handlers
typedef void (*isr_t)(registers_t*);

// Interrupt management functions
bool interrupts_initialize(void);
void register_interrupt_handler(uint8_t n, isr_t handler);
void unregister_interrupt_handler(uint8_t n);

// Enable/disable interrupts
static inline void enable_interrupts(void) {
    asm volatile("sti");
}

static inline void disable_interrupts(void) {
    asm volatile("cli");
}

#endif
