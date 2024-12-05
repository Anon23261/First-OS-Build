#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stddef.h>
#include "multiboot.h"

// VGA text mode constants
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

// VGA colors
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

// Port I/O functions
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Terminal functions
static inline uint8_t make_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static inline uint16_t make_vgaentry(char c, uint8_t color) {
    uint16_t c16 = c;
    uint16_t color16 = color;
    return c16 | color16 << 8;
}

void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_setcolor(uint8_t color);
void terminal_movecursor(size_t x, size_t y);
void terminal_clear(void);

// Kernel state structure
struct KernelState {
    bool vga_initialized;
    bool keyboard_initialized;
    bool filesystem_initialized;
    bool editor_initialized;
    bool memory_initialized;
    bool interrupts_initialized;
    uint16_t* vga_buffer;
    size_t terminal_row;
    size_t terminal_column;
    uint8_t terminal_color;
    const multiboot_info_t* multiboot_info;
    uint32_t total_memory;
    uint32_t free_memory;
};

// External declarations
extern KernelState kernel_state;
extern "C" void kernel_main(multiboot_info_t* mbi);
void kernel_panic(const char* message) __attribute__((noreturn));
bool init_subsystems(const multiboot_info_t* mbi);

#endif /* KERNEL_H */
