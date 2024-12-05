#include "kernel.h"
#include "interrupts.h"
#include "keyboard.h"
#include "memory.h"
#include "string.h"
#include "filesystem.h"
#include "editor.h"
#include <stdint.h>

// VGA text mode constants
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// Kernel state
struct KernelState {
    bool vga_initialized;
    bool keyboard_initialized;
    bool filesystem_initialized;
    bool editor_initialized;
    bool memory_initialized;
    bool interrupts_initialized;
    uint16_t* terminal_buffer;
    size_t terminal_row;
    size_t terminal_column;
    uint8_t terminal_color;
    const multiboot_info_t* multiboot_info;
    uint32_t total_memory;
    uint32_t free_memory;
};

KernelState kernel_state = {
    .vga_initialized = false,
    .keyboard_initialized = false,
    .filesystem_initialized = false,
    .editor_initialized = false,
    .memory_initialized = false,
    .interrupts_initialized = false,
    .terminal_row = 0,
    .terminal_column = 0,
    .terminal_color = 0,
    .terminal_buffer = nullptr,
    .total_memory = 0,
    .free_memory = 0
};

// VGA color enum
enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

// Terminal functions
static uint8_t make_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static uint16_t make_vgaentry(char c, uint8_t color) {
    uint16_t c16 = c;
    uint16_t color16 = color;
    return c16 | color16 << 8;
}

void terminal_initialize() {
    kernel_state.terminal_row = 0;
    kernel_state.terminal_column = 0;
    kernel_state.terminal_color = make_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    kernel_state.terminal_buffer = (uint16_t*) 0xB8000;
    
    // Clear screen
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            kernel_state.terminal_buffer[index] = make_vgaentry(' ', kernel_state.terminal_color);
        }
    }
}

void terminal_scroll() {
    // Move all lines up one
    for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t dst_index = y * VGA_WIDTH + x;
            const size_t src_index = (y + 1) * VGA_WIDTH + x;
            kernel_state.terminal_buffer[dst_index] = kernel_state.terminal_buffer[src_index];
        }
    }

    // Clear bottom line
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
        kernel_state.terminal_buffer[index] = make_vgaentry(' ', kernel_state.terminal_color);
    }

    // Move cursor up one line
    kernel_state.terminal_row--;
}

void terminal_putchar(char c) {
    if (c == '\n') {
        kernel_state.terminal_column = 0;
        if (++kernel_state.terminal_row == VGA_HEIGHT) {
            terminal_scroll();
        }
        return;
    }

    const size_t index = kernel_state.terminal_row * VGA_WIDTH + kernel_state.terminal_column;
    kernel_state.terminal_buffer[index] = make_vgaentry(c, kernel_state.terminal_color);

    if (++kernel_state.terminal_column == VGA_WIDTH) {
        kernel_state.terminal_column = 0;
        if (++kernel_state.terminal_row == VGA_HEIGHT) {
            terminal_scroll();
        }
    }
}

void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        terminal_putchar(data[i]);
    }
}

void terminal_writestring(const char* data) {
    terminal_write(data, strlen(data));
}

// Timer interrupt handler
static void timer_handler(registers_t* regs __attribute__((unused))) {
    static uint32_t tick = 0;
    tick++;
    
    // Update system time every second (assuming 100Hz timer)
    if (tick % 100 == 0) {
        // TODO: Update system time
    }
}

// Initialize all subsystems
bool init_subsystems(const multiboot_info_t* mbi) {
    // Initialize memory management
    if (!memory_initialize(mbi)) {
        return false;
    }
    kernel_state.memory_initialized = true;
    kernel_state.total_memory = memory_get_total();
    kernel_state.free_memory = memory_get_free();
    
    // Initialize terminal first for error reporting
    terminal_initialize();
    kernel_state.vga_initialized = true;
    
    // Initialize interrupt system
    if (!interrupts_initialize()) {
        return false;
    }
    kernel_state.interrupts_initialized = true;
    
    // Register timer interrupt handler
    register_interrupt_handler(IRQ0, timer_handler);
    
    // Initialize keyboard
    keyboard_initialize();
    kernel_state.keyboard_initialized = true;
    
    // Initialize filesystem
    if (!filesystem_initialize()) {
        return false;
    }
    kernel_state.filesystem_initialized = true;
    
    // Initialize text editor
    if (!editor_initialize()) {
        return false;
    }
    kernel_state.editor_initialized = true;
    
    return true;
}

// Kernel panic handler
[[noreturn]] void kernel_panic(const char* message) {
    disable_interrupts();
    
    // Set error color scheme
    kernel_state.terminal_color = make_color(VGA_COLOR_WHITE, VGA_COLOR_RED);
    
    // Clear screen
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            kernel_state.terminal_buffer[index] = make_vgaentry(' ', kernel_state.terminal_color);
        }
    }
    
    // Reset cursor position
    kernel_state.terminal_row = 0;
    kernel_state.terminal_column = 0;
    
    // Print panic message
    terminal_writestring("KERNEL PANIC: ");
    terminal_writestring(message);
    terminal_writestring("\n\nSystem halted.");
    
    // Halt the system
    while (true) {
        asm volatile("hlt");
    }
}

// Kernel entry point
extern "C" void kernel_main(multiboot_info_t* mbi) {
    // Store multiboot info
    kernel_state.multiboot_info = mbi;

    // Check multiboot magic
    if (mbi->magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        kernel_panic("Invalid multiboot magic number");
    }

    // Initialize all subsystems
    if (!init_subsystems(mbi)) {
        kernel_panic("Failed to initialize kernel subsystems");
    }

    // Print welcome message
    terminal_writestring("\nMiniOS booted successfully!\n");
    
    char buf[256];
    snprintf(buf, sizeof(buf), 
             "\nSystem Information:\n"
             "Total Memory: %u KB\n"
             "Free Memory: %u KB\n",
             kernel_state.total_memory / 1024,
             kernel_state.free_memory / 1024);
    terminal_writestring(buf);

    // Enable interrupts
    enable_interrupts();

    // Main kernel loop
    while (true) {
        if (keyboard_has_key()) {
            char c = keyboard_get_key();
            if (kernel_state.editor_initialized) {
                editor_handle_input(c);
            } else {
                terminal_putchar(c);
            }
        }

        // Give CPU some rest
        asm volatile("hlt");
    }
}
