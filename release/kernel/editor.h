#ifndef EDITOR_H
#define EDITOR_H

#include <stddef.h>
#include <stdint.h>
#include "keyboard.h"

// Editor configuration
#define EDITOR_VERSION "1.0.0"
#define TAB_STOP 8
#define QUIT_TIMES 3

// Control key macro (different from keyboard.h KEY_CTRL)
#define EDITOR_CTRL(k) ((k) & 0x1F)

// Editor data structure
typedef struct {
    size_t cursor_x;
    size_t cursor_y;
    size_t screen_rows;
    size_t screen_cols;
    size_t row_offset;
    size_t col_offset;
    char* filename;
    char* buffer;
    size_t buffer_size;
    char status_msg[80];
    uint32_t status_msg_time;
} EditorConfig;

// Function declarations
void editor_init();
void editor_process_keypress();
void editor_refresh_screen();
void editor_set_status_message(const char* fmt, ...);
void editor_open(const char* filename);
void editor_save();
void editor_insert_char(char c);
void editor_delete_char();
void editor_move_cursor(int key);

#endif // EDITOR_H
