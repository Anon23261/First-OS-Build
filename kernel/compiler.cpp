#include "compiler.h"
#include "memory.h"
#include <string.h>

// Simple assembly instruction structure
struct Instruction {
    uint8_t opcode;
    uint8_t operands[4];
    uint8_t operand_count;
};

enum OpCode {
    OP_NOP = 0x00,
    OP_PUSH = 0x01,
    OP_POP = 0x02,
    OP_ADD = 0x03,
    OP_SUB = 0x04,
    OP_MUL = 0x05,
    OP_DIV = 0x06,
    OP_RET = 0x07,
    OP_INT = 0x08
};

extern "C" {
    void* memcpy(void* dest, const void* src, size_t n);
    void* memset(void* dest, int val, size_t n);
    int memcmp(const void* s1, const void* s2, size_t n);
}

static unsigned char* output_buffer = nullptr;
static size_t output_size = 0;

void compiler_init() {
    output_buffer = nullptr;
    output_size = 0;
}

bool tokenize_line(const char* line, Instruction* instr) {
    // Initialize instruction
    instr->opcode = OP_NOP;
    instr->operand_count = 0;
    memset(instr->operands, 0, sizeof(instr->operands));

    // Skip leading whitespace
    while (*line == ' ' || *line == '\t') line++;

    // Empty line or comment
    if (*line == '\0' || *line == '#' || *line == ';') {
        return true;
    }

    // Compare instruction types
    if (memcmp(line, "nop", 3) == 0) {
        instr->opcode = OP_NOP;
    } else if (memcmp(line, "push", 4) == 0) {
        instr->opcode = OP_PUSH;
        line += 4;
        while (*line == ' ' || *line == '\t') line++;
        instr->operands[0] = 0;
        while (*line >= '0' && *line <= '9') {
            instr->operands[0] = instr->operands[0] * 10 + (*line - '0');
            line++;
        }
        instr->operand_count = 1;
    } else if (memcmp(line, "ret", 3) == 0) {
        instr->opcode = OP_RET;
    } else {
        return false;
    }

    return true;
}

CompileResult compile_code(const char* source, size_t length) {
    CompileResult result;
    result.success = false;
    result.binary = nullptr;
    result.binary_size = 0;
    memset(result.error_message, 0, sizeof(result.error_message));
    
    // Allocate temporary storage for instructions
    Instruction* instructions = (Instruction*)memory_allocate(sizeof(Instruction) * length);
    if (!instructions) {
        memcpy(result.error_message, "Failed to allocate memory for compilation", 40);
        return result;
    }

    // Parse source code line by line
    size_t instruction_count = 0;
    const char* line_start = source;
    const char* current = source;
    
    while (current < source + length) {
        if (*current == '\n' || current == source + length - 1) {
            // Process line
            char line_buffer[256];
            size_t line_length = current - line_start;
            if (line_length > 255) line_length = 255;
            memcpy(line_buffer, line_start, line_length);
            line_buffer[line_length] = '\0';
            
            if (!tokenize_line(line_buffer, &instructions[instruction_count])) {
                memcpy(result.error_message, "Invalid instruction", 18);
                memory_free(instructions);
                return result;
            }
            
            instruction_count++;
            line_start = current + 1;
        }
        current++;
    }
    
    // Allocate space for final binary
    result.binary_size = instruction_count * sizeof(Instruction);
    result.binary = (unsigned char*)memory_allocate(result.binary_size);
    if (!result.binary) {
        memcpy(result.error_message, "Failed to allocate memory for final binary", 40);
        memory_free(instructions);
        return result;
    }
    
    // Copy instructions to binary
    memcpy(result.binary, instructions, result.binary_size);
    memory_free(instructions);
    
    result.success = true;
    memcpy(result.error_message, "Compilation successful", 21);
    return result;
}

bool execute_binary(const unsigned char* binary, size_t size) {
    if (!binary || size == 0) return false;
    
    // Create executable memory page
    void* exec_mem = memory_allocate(size);
    if (!exec_mem) return false;
    
    // Copy binary to executable memory
    memcpy(exec_mem, binary, size);
    
    // Execute the binary
    typedef void (*func_ptr)();
    func_ptr func = (func_ptr)exec_mem;
    func();
    
    // Clean up
    memory_free(exec_mem);
    return true;
}
