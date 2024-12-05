#include "stdio.h"
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

int vsnprintf(char* str, size_t size, const char* format, va_list ap) {
    if (!str || !format || size == 0) return -1;
    
    size_t written = 0;
    const char* ptr = format;
    
    while (*ptr && written < size - 1) {
        if (*ptr != '%') {
            str[written++] = *ptr++;
            continue;
        }
        
        ptr++; // Skip '%'
        if (!*ptr) break;
        
        switch (*ptr) {
            case 's': {
                const char* s = va_arg(ap, const char*);
                if (!s) s = "(null)";
                size_t len = strlen(s);
                if (written + len >= size - 1)
                    len = size - written - 1;
                memcpy(str + written, s, len);
                written += len;
                break;
            }
            case 'd': {
                int value = va_arg(ap, int);
                char num[32];
                int len = 0;
                
                if (value < 0) {
                    str[written++] = '-';
                    value = -value;
                }
                
                do {
                    num[len++] = '0' + (value % 10);
                    value /= 10;
                } while (value && len < 31);
                
                while (len > 0 && written < size - 1)
                    str[written++] = num[--len];
                break;
            }
            default:
                str[written++] = *ptr;
                break;
        }
        ptr++;
    }
    
    str[written] = '\0';
    return written;
}

int snprintf(char* str, size_t size, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result = vsnprintf(str, size, format, args);
    va_end(args);
    return result;
}
