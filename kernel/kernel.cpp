#include "kernel.h"
#include "editor.h"
#include "keyboard.h"
#include "filesystem.h"
#include "memory.h"
#include "interrupts.h"
#include <stdio.h>

extern "C" {
    extern int sprintf(char* str, const char* format, ...);
    extern void* memset(void* s, int c, size_t n);
}

// Global kernel state
KernelState kernel_state = {
    .terminal_row = 0,
    .terminal_column = 0,
    .terminal_color = 0,
    .terminal_buffer = nullptr,
    .is_initialized = false,
    .total_memory = 0,
    .free_memory = 0
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

void terminal_init() {
    kernel_state.terminal_row = 0;
    kernel_state.terminal_column = 0;
    kernel_state.terminal_color = make_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    kernel_state.terminal_buffer = (uint16_t*) VGA_MEMORY;
    kernel_state.is_initialized = true;

    terminal_clear();
}

void terminal_clear() {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            kernel_state.terminal_buffer[index] = make_vgaentry(' ', kernel_state.terminal_color);
        }
    }
}

void terminal_movecursor(size_t x, size_t y) {
    uint16_t pos = y * VGA_WIDTH + x;
    outb(0x3D4, 14);
    outb(0x3D5, pos >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, pos & 0xFF);
}

void terminal_set_color(enum vga_color fg, enum vga_color bg) {
    kernel_state.terminal_color = make_color(fg, bg);
}

void terminal_write_char(char c) {
    if (c == '\n') {
        terminal_new_line();
        return;
    }

    const size_t index = kernel_state.terminal_row * VGA_WIDTH + kernel_state.terminal_column;
    kernel_state.terminal_buffer[index] = make_vgaentry(c, kernel_state.terminal_color);

    if (++kernel_state.terminal_column == VGA_WIDTH) {
        terminal_new_line();
    }
    
    terminal_movecursor(kernel_state.terminal_column, kernel_state.terminal_row);
}

void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        terminal_write_char(data[i]);
    }
}

void terminal_write_string(const char* data) {
    for (size_t i = 0; data[i] != '\0'; i++) {
        terminal_write_char(data[i]);
    }
}

void terminal_new_line() {
    kernel_state.terminal_column = 0;
    if (++kernel_state.terminal_row == VGA_HEIGHT) {
        // Scroll up
        for (size_t y = 1; y < VGA_HEIGHT; y++) {
            for (size_t x = 0; x < VGA_WIDTH; x++) {
                const size_t dst_index = (y - 1) * VGA_WIDTH + x;
                const size_t src_index = y * VGA_WIDTH + x;
                kernel_state.terminal_buffer[dst_index] = kernel_state.terminal_buffer[src_index];
            }
        }
        // Clear last line
        kernel_state.terminal_row--;
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = kernel_state.terminal_row * VGA_WIDTH + x;
            kernel_state.terminal_buffer[index] = make_vgaentry(' ', kernel_state.terminal_color);
        }
    }
    terminal_movecursor(kernel_state.terminal_column, kernel_state.terminal_row);
}

void terminal_backspace() {
    if (kernel_state.terminal_column > 0) {
        kernel_state.terminal_column--;
        const size_t index = kernel_state.terminal_row * VGA_WIDTH + kernel_state.terminal_column;
        kernel_state.terminal_buffer[index] = make_vgaentry(' ', kernel_state.terminal_color);
        terminal_movecursor(kernel_state.terminal_column, kernel_state.terminal_row);
    }
}

void terminal_get_size(size_t* rows, size_t* cols) {
    *rows = VGA_HEIGHT;
    *cols = VGA_WIDTH;
}

// Kernel initialization
extern "C" void kernel_init() {
    // Initialize memory management
    memory_init();
    kernel_state.total_memory = memory_get_total();
    kernel_state.free_memory = memory_get_free();
    
    // Initialize terminal
    terminal_init();
    
    // Initialize other subsystems
    keyboard_init();
    filesystem_init();
    interrupts_init();
    editor_init();
}

// Kernel main function
extern "C" void kernel_main() {
    kernel_init();
    
    terminal_write_string("Custom OS v1.1.1\n");
    terminal_write_string("---------------\n");
    terminal_write_string("Memory: ");
    char memstr[32];
    sprintf(memstr, "%uKB total, %uKB free\n", 
            kernel_state.total_memory / 1024,
            kernel_state.free_memory / 1024);
    terminal_write_string(memstr);
    terminal_write_string("\nType 'help' for available commands.\n\n");

    while (true) {
        editor_process_keypress();
    }
}

// Kernel panic function
extern "C" [[noreturn]] void kernel_panic(const char* message) {
    terminal_set_color(VGA_COLOR_RED, VGA_COLOR_BLACK);
    terminal_write_string("\nKERNEL PANIC: ");
    terminal_write_string(message);
    terminal_write_string("\nSystem halted.\n");
    
    while (true) {
        asm volatile("cli; hlt");
    }
}
