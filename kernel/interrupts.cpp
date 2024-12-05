#include "interrupts.h"
#include "kernel.h"
#include "string.h"

// IDT entry structure
struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

// IDT pointer structure
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// Declare IDT and handlers array
static struct idt_entry idt[256];
static isr_t interrupt_handlers[256];
static struct idt_ptr idtp;

// External assembly functions
extern "C" {
    void idt_load(struct idt_ptr* ptr);
    void isr0();
    void isr1();
    // ... Add more ISR declarations as needed
    void irq0();
    void irq1();
    // ... Add more IRQ declarations as needed
}

// Helper function to set an IDT gate
static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = sel;
    idt[num].zero = 0;
    idt[num].flags = flags;
}

// Initialize PIC
static void pic_initialize() {
    // ICW1: start initialization sequence
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    // ICW2: remap IRQ table
    outb(0x21, 0x20);    // IRQ 0-7 -> INT 0x20-0x27
    outb(0xA1, 0x28);    // IRQ 8-15 -> INT 0x28-0x2F

    // ICW3: tell Master PIC that there is a slave PIC at IRQ2
    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    // ICW4: 8086 mode
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    // OCW1: mask all interrupts for now
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);
}

bool interrupts_initialize() {
    // Set up IDT pointer
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t)&idt;

    // Clear IDT and handlers
    memset(&idt, 0, sizeof(struct idt_entry) * 256);
    memset(&interrupt_handlers, 0, sizeof(isr_t) * 256);

    // Set up ISR gates
    idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E);
    idt_set_gate(1, (uint32_t)isr1, 0x08, 0x8E);
    // ... Set up more ISR gates as needed

    // Set up IRQ gates
    idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E);
    idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);
    // ... Set up more IRQ gates as needed

    // Initialize PIC
    pic_initialize();

    // Load IDT
    idt_load(&idtp);

    // Enable interrupts
    enable_interrupts();

    return true;
}

void register_interrupt_handler(uint8_t n, isr_t handler) {
    interrupt_handlers[n] = handler;
    
    // Unmask the interrupt in PIC if it's an IRQ
    if (n >= 32 && n < 48) {
        uint16_t port = (n < 40) ? 0x21 : 0xA1;
        uint8_t mask = inb(port);
        mask &= ~(1 << ((n < 40) ? (n - 32) : (n - 40)));
        outb(port, mask);
    }
}

void unregister_interrupt_handler(uint8_t n) {
    interrupt_handlers[n] = nullptr;
    
    // Mask the interrupt in PIC if it's an IRQ
    if (n >= 32 && n < 48) {
        uint16_t port = (n < 40) ? 0x21 : 0xA1;
        uint8_t mask = inb(port);
        mask |= (1 << ((n < 40) ? (n - 32) : (n - 40)));
        outb(port, mask);
    }
}

// Common interrupt handler
extern "C" void interrupt_handler(registers_t regs) {
    // Send EOI if this is an IRQ
    if (regs.int_no >= 32 && regs.int_no < 48) {
        if (regs.int_no >= 40) {
            outb(0xA0, 0x20);  // Send EOI to slave PIC
        }
        outb(0x20, 0x20);  // Send EOI to master PIC
    }

    // Call the registered handler if one exists
    if (interrupt_handlers[regs.int_no]) {
        interrupt_handlers[regs.int_no](&regs);
    }
}
