#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "multiboot.h"

// Memory management functions
bool memory_initialize(const multiboot_info_t* mbi);
void* memory_allocate(size_t size);
void memory_free(void* ptr);
void* memory_reallocate(void* ptr, size_t size);
uint32_t memory_get_total(void);
uint32_t memory_get_free(void);

// Memory block structure
struct memory_block {
    size_t size;
    bool is_free;
    struct memory_block* next;
    struct memory_block* prev;
};

#endif /* MEMORY_H */
