#include "filesystem.h"
#include "kernel.h"
#include "string.h"
#include <stddef.h>

// File system storage
static struct File files[MAX_FILES];
static size_t num_files = 0;

void filesystem_init() {
    // Initialize all file slots as unused
    for (size_t i = 0; i < MAX_FILES; i++) {
        files[i].used = false;
    }
    num_files = 0;
}

int create_file(const char* filename) {
    if (!filename || strlen(filename) >= MAX_FILENAME_LENGTH) {
        return -1;
    }

    // Check if file already exists
    for (size_t i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strcmp(files[i].name, filename) == 0) {
            return -1;
        }
    }

    // Find free slot
    for (size_t i = 0; i < MAX_FILES; i++) {
        if (!files[i].used) {
            strncpy(files[i].name, filename, MAX_FILENAME_LENGTH - 1);
            files[i].name[MAX_FILENAME_LENGTH - 1] = '\0';
            files[i].size = 0;
            files[i].used = true;
            num_files++;
            return 0;
        }
    }

    return -1;  // No free slots
}

int write_file(const char* filename, const uint8_t* data, size_t size) {
    if (!filename || !data || size > MAX_FILE_SIZE) {
        return -1;
    }

    // Find file
    for (size_t i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strcmp(files[i].name, filename) == 0) {
            memcpy(files[i].data, data, size);
            files[i].size = size;
            return 0;
        }
    }

    // File not found, try to create it
    if (create_file(filename) != 0) {
        return -1;
    }

    // Write to newly created file
    for (size_t i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strcmp(files[i].name, filename) == 0) {
            memcpy(files[i].data, data, size);
            files[i].size = size;
            return 0;
        }
    }

    return -1;
}

int read_file(const char* filename, uint8_t* buffer, size_t* size) {
    if (!filename || !buffer || !size) {
        return -1;
    }

    // Find file
    for (size_t i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strcmp(files[i].name, filename) == 0) {
            memcpy(buffer, files[i].data, files[i].size);
            *size = files[i].size;
            return 0;
        }
    }

    return -1;  // File not found
}

extern "C" char* read_file(const char* filename) {
    // TODO: Implement actual filesystem reading
    // For now, return a dummy buffer
    static char dummy_buffer[1024] = "This is a dummy file content.\n";
    return dummy_buffer;
}

extern "C" int write_file(const char* filename, const char* data, size_t size) {
    // TODO: Implement actual file writing
    // For now, just return success
    return 0;
}

void list_files() {
    terminal_write_string("Files:\n");
    
    if (num_files == 0) {
        terminal_write_string("  No files\n");
        return;
    }

    for (size_t i = 0; i < MAX_FILES; i++) {
        if (files[i].used) {
            char info[MAX_FILENAME_LENGTH + 32];
            snprintf(info, sizeof(info), "  %s (%u bytes)\n", 
                    files[i].name, (unsigned)files[i].size);
            terminal_write_string(info);
        }
    }
}

bool file_exists(const char* filename) {
    if (!filename) return false;

    for (size_t i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strcmp(files[i].name, filename) == 0) {
            return true;
        }
    }

    return false;
}
