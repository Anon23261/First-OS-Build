#include "memory.h"
#include <stddef.h>
#include <stdint.h>

// Simple memory allocator implementation
#define HEAP_START 0x100000
#define HEAP_SIZE  0x100000

struct block_meta {
    size_t size;
    struct block_meta* next;
    bool is_free;
};

static struct block_meta* heap_start = nullptr;

void memory_init() {
    // Initialize the heap
    heap_start = (struct block_meta*)HEAP_START;
    heap_start->size = HEAP_SIZE - sizeof(struct block_meta);
    heap_start->next = nullptr;
    heap_start->is_free = true;
}

static struct block_meta* find_free_block(struct block_meta** last, size_t size) {
    struct block_meta* current = heap_start;
    while (current && !(current->is_free && current->size >= size)) {
        *last = current;
        current = current->next;
    }
    return current;
}

static struct block_meta* request_space(struct block_meta* last, size_t size) {
    struct block_meta* block;
    block = last->next = (struct block_meta*)((char*)last + sizeof(struct block_meta) + last->size);
    block->size = size;
    block->next = nullptr;
    block->is_free = false;
    return block;
}

extern "C" void* malloc(size_t size) {
    if (size == 0) return nullptr;
    
    struct block_meta* block;
    
    // First call to malloc
    if (heap_start == nullptr) {
        memory_init();
    }
    
    struct block_meta* last = heap_start;
    block = find_free_block(&last, size);
    
    if (block == nullptr) {
        // No free block found - allocate new one
        block = request_space(last, size);
        if (block == nullptr) {
            return nullptr;
        }
    } else {
        // Found free block
        block->is_free = false;
    }
    
    return (void*)(block + 1);
}

extern "C" void free(void* ptr) {
    if (ptr == nullptr) return;
    
    // Get the block metadata
    struct block_meta* block_ptr = (struct block_meta*)ptr - 1;
    block_ptr->is_free = true;
    
    // Simple coalescing of adjacent free blocks
    struct block_meta* current = heap_start;
    while (current && current->next) {
        if (current->is_free && current->next->is_free) {
            current->size += sizeof(struct block_meta) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

extern "C" void* realloc(void* ptr, size_t size) {
    if (ptr == nullptr) {
        return malloc(size);
    }
    
    if (size == 0) {
        free(ptr);
        return nullptr;
    }
    
    void* new_ptr = malloc(size);
    if (new_ptr == nullptr) {
        return nullptr;
    }
    
    // Copy old data
    struct block_meta* block_ptr = (struct block_meta*)ptr - 1;
    size_t copy_size = block_ptr->size < size ? block_ptr->size : size;
    memcpy(new_ptr, ptr, copy_size);
    
    free(ptr);
    return new_ptr;
}

extern "C" void* memcpy(void* dest, const void* src, size_t n) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    while (n--) *d++ = *s++;
    return dest;
}

size_t memory_get_total() {
    // TODO: Implement actual memory detection
    return 64 * 1024 * 1024; // Return 64MB for now
}

size_t memory_get_free() {
    return HEAP_SIZE - sizeof(struct block_meta);
}

extern "C" void* memory_allocate(size_t size) {
    return malloc(size);
}

extern "C" void memory_free(void* ptr) {
    free(ptr);
}
