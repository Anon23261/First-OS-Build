#ifndef COMPILER_H
#define COMPILER_H

#include <stddef.h>

// Compilation result structure
struct CompileResult {
    bool success;
    char error_message[256];
    unsigned char* binary;
    size_t binary_size;
};

// Initialize compiler subsystem
void compiler_init();

// Compile source code
CompileResult compile_code(const char* source_code, size_t code_size);

// Execute compiled binary
bool execute_binary(const unsigned char* binary, size_t size);

#endif
