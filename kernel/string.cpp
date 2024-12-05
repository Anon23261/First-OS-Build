#include "string.h"

extern "C" {
    size_t strlen(const char* str) {
        size_t len = 0;
        while (str[len]) len++;
        return len;
    }

    void* memset(void* s, int c, size_t n) {
        unsigned char* p = (unsigned char*)s;
        while (n--) *p++ = (unsigned char)c;
        return s;
    }

    // Remove memcpy implementation since it's in memory.cpp
    extern "C" void* memcpy(void* dest, const void* src, size_t count);

    void* memmove(void* dest, const void* src, size_t n) {
        unsigned char* d = (unsigned char*)dest;
        const unsigned char* s = (const unsigned char*)src;
        if (d < s) {
            while (n--) *d++ = *s++;
        } else {
            d += n;
            s += n;
            while (n--) *--d = *--s;
        }
        return dest;
    }

    int memcmp(const void* s1, const void* s2, size_t n) {
        const unsigned char* p1 = (const unsigned char*)s1;
        const unsigned char* p2 = (const unsigned char*)s2;
        while (n--) {
            if (*p1 != *p2) return *p1 - *p2;
            p1++;
            p2++;
        }
        return 0;
    }

    char* strcpy(char* dest, const char* src) {
        char* d = dest;
        while ((*d++ = *src++));
        return dest;
    }

    char* strncpy(char* dest, const char* src, size_t n) {
        char* d = dest;
        while (n-- && (*d++ = *src++));
        while (n--) *d++ = '\0';
        return dest;
    }

    int strcmp(const char* s1, const char* s2) {
        while (*s1 && *s1 == *s2) {
            s1++;
            s2++;
        }
        return *(unsigned char*)s1 - *(unsigned char*)s2;
    }

    char* strchr(const char* s, int c) {
        while (*s && *s != (char)c) s++;
        return (*s == (char)c) ? (char*)s : nullptr;
    }

    char* strdup(const char* s) {
        size_t len = strlen(s) + 1;
        char* new_str = (char*)malloc(len);
        if (new_str) {
            memcpy(new_str, s, len);
        }
        return new_str;
    }
}
