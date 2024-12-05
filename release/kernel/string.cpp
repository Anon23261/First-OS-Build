#include "string.h"

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

void* memcpy(void* dest, const void* src, size_t n) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

void* memmove(void* dest, const void* src, size_t n) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    if (d < s) {
        for (size_t i = 0; i < n; i++) {
            d[i] = s[i];
        }
    } else {
        for (size_t i = n; i > 0; i--) {
            d[i-1] = s[i-1];
        }
    }
    return dest;
}

void* memset(void* dest, int val, size_t n) {
    uint8_t* d = (uint8_t*)dest;
    for (size_t i = 0; i < n; i++) {
        d[i] = (uint8_t)val;
    }
    return dest;
}

char* strncpy(char* dest, const char* src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i]; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

int iscntrl(int c) {
    return (c >= 0 && c < 32) || c == 127;
}

// Simple printf-style formatting
int vsnprintf(char* str, size_t size, const char* format, va_list ap) {
    if (size == 0) return 0;
    
    size_t pos = 0;
    
    while (*format && pos < size - 1) {
        if (*format != '%') {
            str[pos++] = *format++;
            continue;
        }
        
        format++; // Skip '%'
        
        switch (*format) {
            case 's': {
                const char* s = va_arg(ap, const char*);
                while (*s && pos < size - 1) {
                    str[pos++] = *s++;
                }
                break;
            }
            case 'd': {
                int num = va_arg(ap, int);
                char buf[32];
                int i = 0;
                
                if (num < 0) {
                    str[pos++] = '-';
                    num = -num;
                }
                
                do {
                    buf[i++] = '0' + (num % 10);
                    num /= 10;
                } while (num && i < 32);
                
                while (--i >= 0 && pos < size - 1) {
                    str[pos++] = buf[i];
                }
                break;
            }
            case 'z': {
                format++; // Skip 'u'
                size_t num = va_arg(ap, size_t);
                char buf[32];
                int i = 0;
                
                do {
                    buf[i++] = '0' + (num % 10);
                    num /= 10;
                } while (num && i < 32);
                
                while (--i >= 0 && pos < size - 1) {
                    str[pos++] = buf[i];
                }
                break;
            }
        }
        format++;
    }
    
    str[pos] = '\0';
    return pos;
}

int snprintf(char* str, size_t size, const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    int ret = vsnprintf(str, size, format, ap);
    va_end(ap);
    return ret;
}
