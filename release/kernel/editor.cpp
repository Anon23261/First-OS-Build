#include "editor.h"
#include "keyboard.h"
#include "filesystem.h"
#include "kernel.h"
#include "string.h"
#include <stddef.h>

// Editor state
static struct {
    char* buffer;
    size_t buffer_size;
    size_t cursor_x;
    size_t cursor_y;
    const char* filename;
    size_t screen_rows;
    size_t screen_cols;
} E;

void editor_init() {
    E.buffer = nullptr;
    E.buffer_size = 0;
    E.cursor_x = 0;
    E.cursor_y = 0;
    E.filename = nullptr;
    terminal_get_size(&E.screen_rows, &E.screen_cols);
    if (E.screen_rows == 0) E.screen_rows = 24;
    if (E.screen_cols == 0) E.screen_cols = 80;
    E.screen_rows--; // Make room for status line
}

void editor_open(const char* filename) {
    E.filename = filename;
    char* data = read_file(filename);
    if (data != nullptr) {
        E.buffer = data;
        E.buffer_size = strlen(data);
    }
}

void editor_insert_char(char c) {
    // Reallocate buffer if needed
    size_t new_size = E.buffer_size + 2;  // +1 for new char, +1 for null terminator
    char* new_buffer = (char*)malloc(new_size);
    
    if (new_buffer) {
        // Copy existing content
        if (E.buffer) {
            memcpy(new_buffer, E.buffer, E.buffer_size);
            free(E.buffer);
        }
        
        // Insert new character and null terminator
        new_buffer[E.buffer_size] = c;
        new_buffer[E.buffer_size + 1] = '\0';
        
        E.buffer = new_buffer;
        E.buffer_size = new_size - 1;  // Don't count null terminator in size
        E.cursor_x++;
        
        if (E.cursor_x >= E.screen_cols) {
            E.cursor_x = 0;
            E.cursor_y++;
        }
    }
}

void editor_delete_char() {
    if (E.buffer_size > 0 && E.cursor_x > 0) {
        // Create new buffer with one less character
        size_t new_size = E.buffer_size;
        char* new_buffer = (char*)malloc(new_size);
        
        if (new_buffer) {
            // Copy content before cursor
            memcpy(new_buffer, E.buffer, E.cursor_x - 1);
            
            // Copy content after cursor
            if (E.cursor_x < E.buffer_size) {
                memcpy(new_buffer + E.cursor_x - 1, 
                       E.buffer + E.cursor_x, 
                       E.buffer_size - E.cursor_x);
            }
            
            free(E.buffer);
            E.buffer = new_buffer;
            E.buffer_size = new_size - 1;
            E.cursor_x--;
        }
    }
}

void editor_refresh_screen() {
    terminal_clear();
    
    // Display buffer content
    if (E.buffer) {
        size_t pos = 0;
        size_t row = 0;
        
        while (pos < E.buffer_size && row < E.screen_rows) {
            // Find end of current line
            size_t line_end = pos;
            while (line_end < E.buffer_size && E.buffer[line_end] != '\n') {
                line_end++;
            }
            
            // Calculate line length
            size_t line_length = line_end - pos;
            if (line_length > E.screen_cols) {
                line_length = E.screen_cols;
            }
            
            // Write line to screen
            terminal_write(E.buffer + pos, line_length);
            terminal_write_string("\r\n");
            
            pos = line_end + 1;
            row++;
        }
    } else {
        // Display welcome message
        if (E.screen_rows > 2) {
            char welcome[] = "Editor -- version 2.0.0";
            size_t welcomelen = strlen(welcome);
            if (welcomelen > E.screen_cols) welcomelen = E.screen_cols;
            
            size_t padding = (E.screen_cols - welcomelen) / 2;
            if (padding) {
                terminal_write_char('~');
                padding--;
            }
            while (padding--) terminal_write_char(' ');
            terminal_write_string(welcome);
        }
    }
    
    // Status line
    char status[80];
    int len = snprintf(status, sizeof(status), "%.20s - %zu bytes %d,%d",
        E.filename ? E.filename : "[No Name]", 
        E.buffer_size,
        (int)E.cursor_y + 1,
        (int)E.cursor_x + 1);
    
    if (len > (int)E.screen_cols) len = E.screen_cols;
    
    // Move to last line and print status
    terminal_movecursor(0, E.screen_rows);
    terminal_write_string(status);
    
    // Move cursor back to editing position
    terminal_movecursor(E.cursor_x, E.cursor_y);
}

void editor_process_keypress() {
    char c = keyboard_getchar();
    if (!c) return;  // No character available
    
    switch (c) {
        case '\r':  // Enter key
        case '\n':
            editor_insert_char('\n');
            E.cursor_x = 0;
            E.cursor_y++;
            break;
            
        case 0x08:  // Backspace
            editor_delete_char();
            break;
            
        case 17:  // Ctrl-Q
            terminal_clear();
            terminal_write_string("Exiting editor...\n");
            while(1) {
                asm volatile("hlt");
            }
            break;
            
        case 19:  // Ctrl-S
            if (E.filename && E.buffer) {
                write_file(E.filename, E.buffer, E.buffer_size);
                terminal_write_string("\r\nFile saved.\r\n");
            }
            break;
            
        default:
            if (c >= 32 && c < 127) {
                editor_insert_char(c);
            }
            break;
    }
    
    editor_refresh_screen();
}
