#include "filesystem.h"
#include "kernel.h"
#include <string.h>
#include <stdio.h>

static File files[MAX_FILES];

static struct {
    size_t used_files;
    size_t total_size;
} fs_status;

void init_filesystem() {
    fs_status.used_files = 0;
    fs_status.total_size = 0;
    
    for (int i = 0; i < MAX_FILES; i++) {
        files[i].used = false;
        files[i].size = 0;
        memset(files[i].name, 0, MAX_FILENAME_LENGTH);
        memset(files[i].data, 0, MAX_FILE_SIZE);
    }
}

int create_file(const char* filename) {
    if (!filename || strlen(filename) == 0) {
        return -1;  // Invalid filename
    }
    
    // Check if file already exists
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strcmp(files[i].name, filename) == 0) {
            return -2;  // File already exists
        }
    }
    
    // Find free slot
    int index = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (!files[i].used) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        return -3; // No free slots
    }
    
    strncpy(files[index].name, filename, MAX_FILENAME_LENGTH-1);
    files[index].name[MAX_FILENAME_LENGTH-1] = '\0';
    files[index].size = 0;
    files[index].used = true;
    fs_status.used_files++;
    
    return index;
}

int write_file(const char* filename, const uint8_t* data, size_t size) {
    if (!filename || !data) {
        return -1;  // Invalid parameters
    }
    
    if (size > MAX_FILE_SIZE) {
        return -2;  // File too large
    }
    
    // Find file
    int index = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strcmp(files[i].name, filename) == 0) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        index = create_file(filename);
        if (index < 0) {
            return -3;  // Couldn't create file
        }
    }
    
    fs_status.total_size -= files[index].size;  // Subtract old size
    memcpy(files[index].data, data, size);
    files[index].size = size;
    fs_status.total_size += size;  // Add new size
    
    return 0;  // Success
}

int read_file(const char* filename, uint8_t* buffer, size_t* size) {
    if (!filename || !buffer || !size) {
        return -1;  // Invalid parameters
    }
    
    // Find file
    int index = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strcmp(files[i].name, filename) == 0) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        return -2;  // File not found
    }
    
    *size = files[index].size;
    memcpy(buffer, files[index].data, files[index].size);
    
    return 0;  // Success
}

void list_files() {
    terminal_writestring("Files:\n");
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used) {
            char info[80];
            snprintf(info, sizeof(info), "%s - %d bytes\n", 
                    files[i].name, files[i].size);
            terminal_writestring(info);
        }
    }
    
    char status[80];
    snprintf(status, sizeof(status), "\nTotal: %d files, %d bytes used\n",
             fs_status.used_files, fs_status.total_size);
    terminal_writestring(status);
}

bool file_exists(const char* filename) {
    if (!filename) return false;
    
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strcmp(files[i].name, filename) == 0) {
            return true;
        }
    }
    
    return false;
}
