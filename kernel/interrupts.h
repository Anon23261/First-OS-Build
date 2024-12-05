#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>
#include <stdbool.h>
#include "io.h"

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

#ifdef __cplusplus
extern "C" {
#endif

// Assembly functions
extern "C" void idt_load(struct idt_ptr* ptr);
extern "C" void isr0();
extern "C" void isr1();
extern "C" void irq0();
extern "C" void irq1();

// C handlers
void isr_handler(struct registers* regs);
void irq_handler(struct registers* regs);

// Initialization
void interrupts_init();

#ifdef __cplusplus
}
#endif

#endif
