#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "multiboot.h"

#ifdef __cplusplus
extern "C" {
#endif

// Memory management functions
void memory_init(void);  
void* memory_allocate(size_t size);
void memory_free(void* ptr);
void* memory_reallocate(void* ptr, size_t size);
uint32_t memory_get_total(void);
uint32_t memory_get_free(void);

void* malloc(size_t size);
void free(void* ptr);
void* realloc(void* ptr, size_t size);
void* memcpy(void* dest, const void* src, size_t n);

// Memory block structure
struct memory_block {
    size_t size;
    bool is_free;
    struct memory_block* next;
    struct memory_block* prev;
};

#ifdef __cplusplus
}
#endif

#endif /* MEMORY_H */
