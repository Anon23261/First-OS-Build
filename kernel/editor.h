#ifndef EDITOR_H
#define EDITOR_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Editor configuration
struct EditorConfig {
    char* buffer;
    size_t buffer_size;
    char* filename;
    size_t screen_rows;
    size_t screen_cols;
    size_t cursor_x;
    size_t cursor_y;
    size_t row_offset;
    size_t col_offset;
    char status_msg[80];
    uint64_t status_msg_time;
};

// Editor functions
void editor_init(void);
void editor_refresh_screen(void);
void editor_open(const char* filename);
void editor_insert_char(char c);
void editor_process_keypress(void);

#ifdef __cplusplus
}
#endif

#endif // EDITOR_H
