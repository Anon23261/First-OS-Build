#include "interrupts.h"
#include "kernel.h"
#include "keyboard.h"
#include <stddef.h>
#include <string.h>

// Forward declarations
void int_to_string(int num, char* str);
static void timer_handler(struct registers* regs);
void keyboard_handler(struct registers* regs);

// IDT entry structure
struct idt_entry {
    uint16_t base_lo;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t base_hi;
} __attribute__((packed));

// IDT pointer structure
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// IDT array
static struct idt_entry idt[256];
static struct idt_ptr idtp;

// Timer variables
static volatile uint32_t tick = 0;

// Function to set an IDT gate
static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_lo = (base & 0xFFFF);
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

// External assembly functions
extern "C" {
    void idt_load(struct idt_ptr* ptr);
    void isr0();
    void isr1();
    void irq0();
    void irq1();
}

// Timer handler implementation
static void timer_handler(struct registers* regs) {
    (void)regs; // Unused parameter
    tick++;
}

// ISR handlers
extern "C" void isr_handler(struct registers* regs) {
    // Handle CPU exceptions here
    if (regs->int_no < 32) {
        char num_str[12];
        int_to_string(regs->int_no, num_str);
        kernel_panic(num_str);
    }
}

extern "C" void irq_handler(struct registers* regs) {
    // Send EOI signal to PICs
    if (regs->int_no >= 40) {
        outb(0xA0, 0x20);
    }
    outb(0x20, 0x20);

    // Handle IRQs
    switch (regs->int_no - 32) {
        case 0:  // Timer
            timer_handler(regs);
            break;
        case 1:  // Keyboard
            keyboard_handler(regs);
            break;
    }
}

// Initialize PIC
static void pic_init() {
    // Remap PIC
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}

// Initialize interrupt descriptor table
extern "C" void interrupts_init() {
    // Set up IDT pointer
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t)&idt;

    // Clear IDT
    memset(&idt, 0, sizeof(struct idt_entry) * 256);

    // Initialize PIC
    pic_init();

    // Set up ISR gates
    idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E);
    idt_set_gate(1, (uint32_t)isr1, 0x08, 0x8E);

    // Set up IRQ gates
    idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E);  // Timer
    idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);  // Keyboard

    // Load IDT
    idt_load(&idtp);

    // Enable interrupts
    asm volatile("sti");
}

// Helper function to convert int to string
void int_to_string(int num, char* str) {
    int i = 0;
    bool isNegative = false;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    if (num < 0) {
        isNegative = true;
        num = -num;
    }

    while (num != 0) {
        int rem = num % 10;
        str[i++] = rem + '0';
        num = num / 10;
    }

    if (isNegative) {
        str[i++] = '-';
    }

    str[i] = '\0';

    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}
