#include "editor.h"
#include "keyboard.h"
#include "filesystem.h"
#include "string.h"
#include "kernel.h"

#define EDITOR_BUFFER_SIZE 4096
#define MAX_FILENAME_LENGTH 256

struct EditorState {
    char buffer[EDITOR_BUFFER_SIZE];
    size_t cursor_pos;
    size_t buffer_length;
    char current_file[MAX_FILENAME_LENGTH];
    bool file_loaded;
    bool modified;
};

static EditorState editor_state;

bool editor_initialize() {
    memset(&editor_state, 0, sizeof(EditorState));
    editor_state.cursor_pos = 0;
    editor_state.buffer_length = 0;
    editor_state.file_loaded = false;
    editor_state.modified = false;
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
        editor_state.buffer_length = 0;
        editor_state.cursor_pos = 0;
        editor_state.file_loaded = true;
        editor_state.modified = false;
        strncpy(editor_state.current_file, filename, MAX_FILENAME_LENGTH - 1);
        editor_state.current_file[MAX_FILENAME_LENGTH - 1] = '\0';
        return true;
    }

    // Read file contents
    size_t bytes_read = filesystem_read(file, editor_state.buffer, EDITOR_BUFFER_SIZE - 1);
    filesystem_close(file);

    if (bytes_read == SIZE_MAX) {
        return false;
    }

    editor_state.buffer[bytes_read] = '\0';
    editor_state.buffer_length = bytes_read;
    editor_state.cursor_pos = bytes_read;
    editor_state.file_loaded = true;
    editor_state.modified = false;
    strncpy(editor_state.current_file, filename, MAX_FILENAME_LENGTH - 1);
    editor_state.current_file[MAX_FILENAME_LENGTH - 1] = '\0';

    return true;
}

bool editor_save() {
    if (!editor_state.file_loaded) {
        return false;
    }

    FileHandle* file = filesystem_open(editor_state.current_file, "w");
    if (!file) {
        return false;
    }

    size_t bytes_written = filesystem_write(file, editor_state.buffer, editor_state.buffer_length);
    filesystem_close(file);

    if (bytes_written != editor_state.buffer_length) {
        return false;
    }

    editor_state.modified = false;
    return true;
}

void editor_insert_char(char c) {
    if (!editor_state.file_loaded || editor_state.buffer_length >= EDITOR_BUFFER_SIZE - 1) {
        return;
    }

    // Make room for the new character
    for (size_t i = editor_state.buffer_length; i > editor_state.cursor_pos; i--) {
        editor_state.buffer[i] = editor_state.buffer[i - 1];
    }

    // Insert the character
    editor_state.buffer[editor_state.cursor_pos] = c;
    editor_state.cursor_pos++;
    editor_state.buffer_length++;
    editor_state.modified = true;

    // Ensure null termination
    editor_state.buffer[editor_state.buffer_length] = '\0';
}

void editor_delete_char() {
    if (!editor_state.file_loaded || editor_state.cursor_pos == 0) {
        return;
    }

    // Shift characters left
    for (size_t i = editor_state.cursor_pos - 1; i < editor_state.buffer_length - 1; i++) {
        editor_state.buffer[i] = editor_state.buffer[i + 1];
    }

    editor_state.cursor_pos--;
    editor_state.buffer_length--;
    editor_state.modified = true;

    // Ensure null termination
    editor_state.buffer[editor_state.buffer_length] = '\0';
}

void editor_handle_input(char c) {
    if (!editor_state.file_loaded) {
        return;
    }

    if (c == '\b') {
        editor_delete_char();
    } else if (c == '\n') {
        editor_insert_char('\n');
    } else if (c >= ' ' && c <= '~') {
        editor_insert_char(c);
    }
}

const char* editor_get_buffer() {
    return editor_state.buffer;
}

size_t editor_get_cursor_pos() {
    return editor_state.cursor_pos;
}

bool editor_is_modified() {
    return editor_state.modified;
}

const char* editor_get_filename() {
    return editor_state.current_file;
}

void editor_render() {
    terminal_clear();
    terminal_movecursor(0, 0);
    terminal_writestring(editor_get_buffer());
    terminal_movecursor(editor_get_cursor_pos(), 0);
}

void editor_process_keypress() {
    char c = get_key();
    if (!c) return;

    switch (c) {
        case KEY_CTRL('q'):
            terminal_clear();
            terminal_movecursor(0, 0);
            kernel_panic("Editor exited");
            break;

        case KEY_CTRL('s'):
            editor_save();
            break;

        default:
            editor_handle_input(c);
            break;
    }

    editor_render();
}
