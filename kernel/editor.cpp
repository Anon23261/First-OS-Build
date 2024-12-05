#include "editor.h"
#include "keyboard.h"
#include "filesystem.h"
#include "kernel.h"
#include "string.h"

extern "C" {
    extern void terminal_clear(void);
    extern void terminal_write_char(char c);
    extern void terminal_write_string(const char* s);
    extern void terminal_write(const char* data, size_t size);
    extern void terminal_movecursor(size_t x, size_t y);
    extern int snprintf(char* str, size_t size, const char* format, ...);
    extern void* malloc(size_t size);
    extern void* realloc(void* ptr, size_t size);
    extern void free(void* ptr);
    extern void* memset(void* s, int c, size_t n);
    extern void* memmove(void* dest, const void* src, size_t n);
    extern size_t strlen(const char* s);
    extern char* strchr(const char* s, int c);
    extern char* strdup(const char* s);
}

// Editor state
EditorConfig E;

void editor_init() {
    E.cursor_x = 0;
    E.cursor_y = 0;
    E.screen_rows = 24;  // Default terminal size
    E.screen_cols = 80;
    E.row_offset = 0;
    E.col_offset = 0;
    E.filename = nullptr;
    E.buffer = nullptr;
    E.buffer_size = 0;
    memset(E.status_msg, 0, sizeof(E.status_msg));
    E.status_msg_time = 0;
}

static size_t get_num_lines(const char* buffer) {
    if (!buffer) return 0;
    size_t count = 1;
    for (size_t i = 0; i < strlen(buffer); i++) {
        if (buffer[i] == '\n') count++;
    }
    return count;
}

static const char* get_line(const char* buffer, size_t line) {
    if (!buffer) return nullptr;
    const char* current = buffer;
    for (size_t i = 0; i < line; i++) {
        current = strchr(current, '\n');
        if (!current) return nullptr;
        current++;
    }
    return current;
}

void editor_refresh_screen() {
    terminal_clear();
    
    for (size_t y = 0; y < E.screen_rows - 1; y++) {
        size_t filerow = y + E.row_offset;
        
        if (filerow >= get_num_lines(E.buffer)) {
            if (E.buffer_size == 0 && y == E.screen_rows / 3) {
                char welcome[80];
                int welcomelen = snprintf(welcome, sizeof(welcome),
                    "Editor -- version 1.0.0");
                if (welcomelen > (int)E.screen_cols) welcomelen = E.screen_cols;
                
                size_t padding = (E.screen_cols - welcomelen) / 2;
                while (padding--) terminal_write_char(' ');
                terminal_write_string(welcome);
            } else {
                terminal_write_char('~');
            }
        } else {
            const char* line = get_line(E.buffer, filerow);
            if (line) {
                size_t len = 0;
                const char* eol = strchr(line, '\n');
                if (eol) len = eol - line;
                else len = strlen(line);
                
                if (len > E.col_offset) {
                    len -= E.col_offset;
                    if (len > E.screen_cols) len = E.screen_cols;
                    terminal_write(line + E.col_offset, len);
                }
            }
        }
        terminal_write_string("\r\n");
    }
    
    // Status line
    char status[80];
    int len = snprintf(status, sizeof(status), "%.20s - %zu lines",
        E.filename ? E.filename : "[No Name]", get_num_lines(E.buffer));
    
    if (len > (int)E.screen_cols) len = E.screen_cols;
    terminal_write_string(status);
    
    // Move cursor to current position
    terminal_movecursor(E.cursor_x - E.col_offset, E.cursor_y - E.row_offset);
}

void editor_open(const char* filename) {
    if (E.buffer) {
        free(E.buffer);
        E.buffer = nullptr;
        E.buffer_size = 0;
    }
    
    uint8_t* data = nullptr;
    size_t size = 0;
    if (read_file(filename, &data, &size) == 0 && data != nullptr) {
        E.buffer = (char*)data;
        E.buffer_size = size;
        if (E.filename) free(E.filename);
        E.filename = strdup(filename);
    }
}

void editor_insert_char(char c) {
    if (!E.buffer) {
        E.buffer = (char*)malloc(1);
        if (!E.buffer) return;
        E.buffer_size = 0;
    }
    
    char* new_buffer = (char*)realloc(E.buffer, E.buffer_size + 2);
    if (!new_buffer) return;
    E.buffer = new_buffer;
    
    memmove(&E.buffer[E.cursor_x + 1], &E.buffer[E.cursor_x], E.buffer_size - E.cursor_x);
    E.buffer[E.cursor_x] = c;
    E.buffer_size++;
    E.cursor_x++;
    E.buffer[E.buffer_size] = '\0';
}

void editor_process_keypress() {
    int c = keyboard_getchar();
    if (!c) return;

    switch (c) {
        case 17:  // Ctrl-Q
            terminal_clear();
            while(1) {
                asm volatile("hlt");
            }
            break;
            
        case 19:  // Ctrl-S
            if (E.filename) {
                write_file(E.filename, (uint8_t*)E.buffer, E.buffer_size);
            }
            break;
            
        default:
            if (c >= 32 && c < 127) {
                editor_insert_char((char)c);
            }
            break;
    }
    
    editor_refresh_screen();
}
