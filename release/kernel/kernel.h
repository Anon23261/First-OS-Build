#ifndef KERNEL_H
#define KERNEL_H

#include <stddef.h>
#include <stdint.h>

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

// Terminal dimensions
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

// Kernel state structure
typedef struct {
    size_t terminal_row;
    size_t terminal_column;
    uint8_t terminal_color;
    uint16_t* terminal_buffer;
    bool is_initialized;
} KernelState;

// Function declarations
void kernel_init(void);
void kernel_main(void);
void terminal_init(void);
void terminal_clear(void);
void terminal_set_color(enum vga_color fg, enum vga_color bg);
void terminal_write_char(char c);
void terminal_write(const char* data, size_t size);
void terminal_write_string(const char* data);
void terminal_new_line(void);
void terminal_backspace(void);
void terminal_get_size(size_t* rows, size_t* cols);
uint32_t kernel_get_ticks(void);

[[noreturn]] void kernel_panic(const char* message);

#endif // KERNEL_H
