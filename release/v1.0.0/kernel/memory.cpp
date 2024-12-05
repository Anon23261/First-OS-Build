#include "memory.h"
#include "kernel.h"
#include <stdint.h>
#include <string.h>

// Memory management globals
static struct memory_block* heap_start = nullptr;
static uint32_t total_memory = 0;
static uint32_t free_memory = 0;

// Helper functions
static void* align_address(void* addr, size_t alignment) {
    return (void*)(((uintptr_t)addr + (alignment - 1)) & ~(alignment - 1));
}

static struct memory_block* find_free_block(size_t size) {
    struct memory_block* current = heap_start;
    while (current) {
        if (current->is_free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

static void merge_free_blocks(struct memory_block* block) {
    // Merge with next block if it's free
    while (block->next && block->next->is_free) {
        block->size += block->next->size + sizeof(struct memory_block);
        block->next = block->next->next;
        if (block->next) {
            block->next->prev = block;
        }
    }
    
    // Merge with previous block if it's free
    while (block->prev && block->prev->is_free) {
        block = block->prev;
        block->size += block->next->size + sizeof(struct memory_block);
        block->next = block->next->next;
        if (block->next) {
            block->next->prev = block;
        }
    }
}

bool memory_initialize(const multiboot_info_t* mbi) {
    if (!mbi) return false;

    // Get memory information from multiboot
    if (!(mbi->flags & MULTIBOOT_INFO_MEMORY)) {
        return false;
    }

    total_memory = (mbi->mem_lower + mbi->mem_upper) * 1024;  // Convert KB to bytes
    
    // Initialize heap
    // Start after kernel (1MB + kernel size, aligned to 4KB)
    uintptr_t heap_addr = 0x100000;  // 1MB
    heap_addr = (uintptr_t)align_address((void*)heap_addr, 4096);
    
    heap_start = (struct memory_block*)heap_addr;
    heap_start->size = total_memory - heap_addr - sizeof(struct memory_block);
    heap_start->is_free = true;
    heap_start->next = nullptr;
    heap_start->prev = nullptr;
    
    free_memory = heap_start->size;
    
    return true;
}

void* memory_allocate(size_t size) {
    if (size == 0) return nullptr;
    
    // Align size to 8 bytes
    size = (size + 7) & ~7;
    
    struct memory_block* block = find_free_block(size);
    if (!block) return nullptr;
    
    // Split block if it's too large
    if (block->size > size + sizeof(struct memory_block) + 8) {
        struct memory_block* new_block = (struct memory_block*)((char*)block + sizeof(struct memory_block) + size);
        new_block->size = block->size - size - sizeof(struct memory_block);
        new_block->is_free = true;
        new_block->next = block->next;
        new_block->prev = block;
        
        if (block->next) {
            block->next->prev = new_block;
        }
        
        block->next = new_block;
        block->size = size;
    }
    
    block->is_free = false;
    free_memory -= block->size + sizeof(struct memory_block);
    
    return (void*)((char*)block + sizeof(struct memory_block));
}

void memory_free(void* ptr) {
    if (!ptr) return;
    
    struct memory_block* block = (struct memory_block*)((char*)ptr - sizeof(struct memory_block));
    block->is_free = true;
    free_memory += block->size + sizeof(struct memory_block);
    
    merge_free_blocks(block);
}

void* memory_reallocate(void* ptr, size_t size) {
    if (!ptr) return memory_allocate(size);
    if (size == 0) {
        memory_free(ptr);
        return nullptr;
    }
    
    struct memory_block* block = (struct memory_block*)((char*)ptr - sizeof(struct memory_block));
    
    // If the current block is big enough, just return it
    if (block->size >= size) {
        return ptr;
    }
    
    // Allocate new block
    void* new_ptr = memory_allocate(size);
    if (!new_ptr) return nullptr;
    
    // Copy old data
    memcpy(new_ptr, ptr, block->size);
    
    // Free old block
    memory_free(ptr);
    
    return new_ptr;
}

uint32_t memory_get_total(void) {
    return total_memory;
}

uint32_t memory_get_free(void) {
    return free_memory;
}
