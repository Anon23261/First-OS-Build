#ifndef EDITOR_H
#define EDITOR_H

#include <stddef.h>
#include <stdint.h>

// Editor dimensions
#define EDITOR_ROWS 23  // Leave space for status bar
#define EDITOR_COLS 80
#define EDITOR_TAB_SIZE 4
#define MAX_LINE_LENGTH 256

// Special key definitions
#define KEY_CTRL(k) ((k) & 0x1F)
#define KEY_ARROW_LEFT  0x4B
#define KEY_ARROW_RIGHT 0x4D
#define KEY_ARROW_UP    0x48
#define KEY_ARROW_DOWN  0x50
#define KEY_PAGE_UP     0x49
#define KEY_PAGE_DOWN   0x51
#define KEY_HOME        0x47
#define KEY_END         0x4F
#define KEY_DELETE      0x53
#define KERNEL_VERSION_STRING "1.0.0"

struct EditorRow {
    char chars[MAX_LINE_LENGTH];
    size_t size;
};

struct EditorConfig {
    size_t cx, cy;          // Cursor position
    size_t rowoff;          // Row offset for scrolling
    size_t coloff;          // Column offset for scrolling
    EditorRow rows[EDITOR_ROWS];
    size_t numrows;
    char filename[32];
    char statusmsg[80];
    bool dirty;             // Whether file has unsaved changes
};

bool editor_initialize(void);
void editor_process_keypress(void);
void editor_render(void);
void editor_open(const char* filename);
void editor_save(void);
void editor_insert_char(char c);
void editor_delete_char(void);
void editor_insert_newline(void);
void editor_set_status_message(const char* fmt, ...);
void editor_handle_input(char c);

#endif /* EDITOR_H */
