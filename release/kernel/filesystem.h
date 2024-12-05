#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stddef.h>
#include <stdint.h>

#define MAX_FILES 256
#define MAX_FILENAME_LENGTH 32
#define MAX_FILE_SIZE 4096

struct File {
    char name[MAX_FILENAME_LENGTH];
    uint8_t data[MAX_FILE_SIZE];
    size_t size;
    bool used;
};

bool filesystem_initialize(void);
int create_file(const char* filename);
int write_file(const char* filename, const uint8_t* data, size_t size);
int read_file(const char* filename, uint8_t* buffer, size_t* size);
void list_files(void);
bool file_exists(const char* filename);

#endif /* FILESYSTEM_H */
