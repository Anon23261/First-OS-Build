#include "editor.h"
#include "keyboard.h"
#include "filesystem.h"
#include "string.h"
#include "kernel.h"

#define EDITOR_BUFFER_SIZE 4096
#define MAX_FILENAME_LENGTH 256
#define EDITOR_VERSION "1.0.0"
#define EDITOR_TAB_STOP 4

struct EditorState {
    char buffer[EDITOR_BUFFER_SIZE];
    size_t cursor_x;
    size_t cursor_y;
    size_t screen_rows;
    size_t screen_cols;
    size_t row_offset;
    size_t col_offset;
    size_t buffer_length;
    char current_file[MAX_FILENAME_LENGTH];
    bool file_loaded;
    bool modified;
    char status_msg[80];
    uint32_t status_msg_time;
};

static EditorState E;

// Terminal operations
static void clear_screen() {
    terminal_clear();
}

static void refresh_screen() {
    // Hide cursor during refresh
    terminal_movecursor(999, 999);

    // Draw rows
    for (size_t y = 0; y < E.screen_rows - 1; y++) {
        size_t filerow = y + E.row_offset;
        
        if (filerow >= get_num_lines(E.buffer)) {
            if (y == E.screen_rows / 3 && !E.file_loaded) {
                char welcome[80];
                int welcomelen = snprintf(welcome, sizeof(welcome),
                    "Editor -- version %s", EDITOR_VERSION);
                if (welcomelen > E.screen_cols) welcomelen = E.screen_cols;
                
                size_t padding = (E.screen_cols - welcomelen) / 2;
                if (padding) {
                    terminal_putchar('~');
                    padding--;
                }
                while (padding--) terminal_putchar(' ');
                
                terminal_writestring(welcome);
            } else {
                terminal_putchar('~');
            }
        } else {
            size_t len = get_line_length(E.buffer, filerow);
            if (len > E.col_offset) {
                len -= E.col_offset;
                if (len > E.screen_cols) len = E.screen_cols;
                terminal_write(get_line(E.buffer, filerow) + E.col_offset, len);
            }
        }
        
        terminal_writestring("\r\n");
    }

    // Status bar
    terminal_set_color(VGA_COLOR_BLACK, VGA_COLOR_WHITE);
    char status[80];
    char rstatus[30];
    
    int len = snprintf(status, sizeof(status), "%.20s - %zu lines %s",
        E.current_file[0] ? E.current_file : "[No Name]", 
        get_num_lines(E.buffer),
        E.modified ? "(modified)" : "");
    
    int rlen = snprintf(rstatus, sizeof(rstatus), "%zu/%zu",
        E.cursor_y + 1, get_num_lines(E.buffer));
    
    if (len > E.screen_cols) len = E.screen_cols;
    terminal_writestring(status);
    
    while (len < E.screen_cols) {
        if (E.screen_cols - len == rlen) {
            terminal_writestring(rstatus);
            break;
        } else {
            terminal_putchar(' ');
            len++;
        }
    }
    terminal_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

    // Status message
    if (E.status_msg[0] && (kernel_get_ticks() - E.status_msg_time) < 5000) {
        terminal_writestring(E.status_msg);
    }

    // Position cursor
    terminal_movecursor(E.cursor_x - E.col_offset, E.cursor_y - E.row_offset);
}

void editor_set_status_message(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(E.status_msg, sizeof(E.status_msg), fmt, ap);
    va_end(ap);
    E.status_msg_time = kernel_get_ticks();
}

bool editor_initialize() {
    E.cursor_x = 0;
    E.cursor_y = 0;
    E.row_offset = 0;
    E.col_offset = 0;
    E.buffer_length = 0;
    E.file_loaded = false;
    E.modified = false;
    E.status_msg[0] = '\0';
    E.status_msg_time = 0;
    
    // Get screen size
    terminal_get_size(&E.screen_rows, &E.screen_cols);
    E.screen_rows -= 2;  // Make room for status bar and status message
    
    return true;
}

bool editor_open(const char* filename) {
    if (!filename || strlen(filename) >= MAX_FILENAME_LENGTH) {
        return false;
    }

    // Try to open the file
    FileHandle* file = filesystem_open(filename, "r");
    if (!file) {
        // New file
        E.buffer_length = 0;
        E.cursor_x = 0;
        E.cursor_y = 0;
        E.file_loaded = true;
        E.modified = false;
        strncpy(E.current_file, filename, MAX_FILENAME_LENGTH - 1);
        E.current_file[MAX_FILENAME_LENGTH - 1] = '\0';
        editor_set_status_message("New file: %s", filename);
        return true;
    }

    // Read file contents
    size_t bytes_read = filesystem_read(file, E.buffer, EDITOR_BUFFER_SIZE - 1);
    filesystem_close(file);

    if (bytes_read == SIZE_MAX) {
        editor_set_status_message("Error reading file: %s", filename);
        return false;
    }

    E.buffer[bytes_read] = '\0';
    E.buffer_length = bytes_read;
    E.cursor_x = 0;
    E.cursor_y = 0;
    E.file_loaded = true;
    E.modified = false;
    strncpy(E.current_file, filename, MAX_FILENAME_LENGTH - 1);
    E.current_file[MAX_FILENAME_LENGTH - 1] = '\0';
    
    editor_set_status_message("Loaded %zu bytes from %s", bytes_read, filename);
    return true;
}

bool editor_save() {
    if (!E.file_loaded) {
        editor_set_status_message("No file loaded");
        return false;
    }

    FileHandle* file = filesystem_open(E.current_file, "w");
    if (!file) {
        editor_set_status_message("Can't open file for writing: %s", E.current_file);
        return false;
    }

    size_t bytes_written = filesystem_write(file, E.buffer, E.buffer_length);
    filesystem_close(file);

    if (bytes_written != E.buffer_length) {
        editor_set_status_message("Error writing to file: %s", E.current_file);
        return false;
    }

    E.modified = false;
    editor_set_status_message("%zu bytes written to %s", bytes_written, E.current_file);
    return true;
}

void editor_move_cursor(int key) {
    size_t line_length = get_current_line_length(E.buffer, E.cursor_y);
    
    switch (key) {
        case KEY_ARROW_LEFT:
            if (E.cursor_x > 0) {
                E.cursor_x--;
            } else if (E.cursor_y > 0) {
                E.cursor_y--;
                E.cursor_x = get_line_length(E.buffer, E.cursor_y);
            }
            break;
            
        case KEY_ARROW_RIGHT:
            if (E.cursor_x < line_length) {
                E.cursor_x++;
            } else if (E.cursor_y < get_num_lines(E.buffer) - 1) {
                E.cursor_y++;
                E.cursor_x = 0;
            }
            break;
            
        case KEY_ARROW_UP:
            if (E.cursor_y > 0) {
                E.cursor_y--;
                size_t new_line_length = get_line_length(E.buffer, E.cursor_y);
                if (E.cursor_x > new_line_length) {
                    E.cursor_x = new_line_length;
                }
            }
            break;
            
        case KEY_ARROW_DOWN:
            if (E.cursor_y < get_num_lines(E.buffer) - 1) {
                E.cursor_y++;
                size_t new_line_length = get_line_length(E.buffer, E.cursor_y);
                if (E.cursor_x > new_line_length) {
                    E.cursor_x = new_line_length;
                }
            }
            break;
    }
}

void editor_scroll() {
    // Vertical scrolling
    if (E.cursor_y < E.row_offset) {
        E.row_offset = E.cursor_y;
    }
    if (E.cursor_y >= E.row_offset + E.screen_rows) {
        E.row_offset = E.cursor_y - E.screen_rows + 1;
    }
    
    // Horizontal scrolling
    if (E.cursor_x < E.col_offset) {
        E.col_offset = E.cursor_x;
    }
    if (E.cursor_x >= E.col_offset + E.screen_cols) {
        E.col_offset = E.cursor_x - E.screen_cols + 1;
    }
}

void editor_insert_char(char c) {
    if (!E.file_loaded || E.buffer_length >= EDITOR_BUFFER_SIZE - 1) {
        return;
    }

    // Make room for the new character
    for (size_t i = E.buffer_length; i > E.cursor_x; i--) {
        E.buffer[i] = E.buffer[i - 1];
    }

    // Insert the character
    E.buffer[E.cursor_x] = c;
    E.cursor_x++;
    E.buffer_length++;
    E.modified = true;

    // Ensure null termination
    E.buffer[E.buffer_length] = '\0';
}

void editor_delete_char() {
    if (!E.file_loaded || E.cursor_x == 0) {
        return;
    }

    // Shift characters left
    for (size_t i = E.cursor_x - 1; i < E.buffer_length - 1; i++) {
        E.buffer[i] = E.buffer[i + 1];
    }

    E.cursor_x--;
    E.buffer_length--;
    E.modified = true;

    // Ensure null termination
    E.buffer[E.buffer_length] = '\0';
}

void editor_insert_newline() {
    if (!E.file_loaded || E.buffer_length >= EDITOR_BUFFER_SIZE - 1) {
        return;
    }

    // Make room for the new character
    for (size_t i = E.buffer_length; i > E.cursor_x; i--) {
        E.buffer[i] = E.buffer[i - 1];
    }

    // Insert the character
    E.buffer[E.cursor_x] = '\n';
    E.cursor_x++;
    E.buffer_length++;
    E.cursor_y++;
    E.cursor_x = 0;
    E.modified = true;

    // Ensure null termination
    E.buffer[E.buffer_length] = '\0';
}

void editor_process_keypress() {
    char c = keyboard_getchar();
    if (!c) return;

    switch (c) {
        case KEY_CTRL('q'):
            if (E.modified) {
                editor_set_status_message("Warning! File has unsaved changes. "
                    "Press Ctrl-Q again to quit.");
                E.modified = false;
                return;
            }
            clear_screen();
            terminal_movecursor(0, 0);
            kernel_panic("Editor exited");
            break;

        case KEY_CTRL('s'):
            editor_save();
            break;

        case KEY_ARROW_UP:
        case KEY_ARROW_DOWN:
        case KEY_ARROW_LEFT:
        case KEY_ARROW_RIGHT:
            editor_move_cursor(c);
            break;

        case KEY_PAGE_UP:
        case KEY_PAGE_DOWN:
            {
                size_t times = E.screen_rows;
                while (times--)
                    editor_move_cursor(c == KEY_PAGE_UP ? KEY_ARROW_UP : KEY_ARROW_DOWN);
            }
            break;

        case KEY_HOME:
            E.cursor_x = 0;
            break;

        case KEY_END:
            E.cursor_x = get_current_line_length(E.buffer, E.cursor_y);
            break;

        case '\r':
            editor_insert_newline();
            break;

        case KEY_CTRL('h'):
        case '\b':
            editor_delete_char();
            break;

        case KEY_DELETE:
            editor_move_cursor(KEY_ARROW_RIGHT);
            editor_delete_char();
            break;

        default:
            if (!iscntrl(c)) {
                editor_insert_char(c);
            }
            break;
    }

    editor_scroll();
    refresh_screen();
}
