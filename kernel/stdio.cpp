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

#include <stdarg.h>
#include <stddef.h>
#include "stdio.h"

extern "C" {

void reverse(char* str, int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

char* itoa(int num, char* str, int base) {
    int i = 0;
    bool isNegative = false;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    if (num < 0 && base == 10) {
        isNegative = true;
        num = -num;
    }

    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    if (isNegative)
        str[i++] = '-';

    str[i] = '\0';
    reverse(str, i);
    return str;
}

int sprintf(char* str, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    int written = 0;
    char num_buffer[32];
    
    while (*format != '\0') {
        if (*format != '%') {
            str[written++] = *format++;
            continue;
        }
        
        format++;
        switch (*format) {
            case 'd':
            case 'i': {
                int val = va_arg(args, int);
                itoa(val, num_buffer, 10);
                for (char* p = num_buffer; *p; p++) {
                    str[written++] = *p;
                }
                break;
            }
            case 's': {
                char* val = va_arg(args, char*);
                while (*val) {
                    str[written++] = *val++;
                }
                break;
            }
            case 'c': {
                char val = (char)va_arg(args, int);
                str[written++] = val;
                break;
            }
            case '%': {
                str[written++] = '%';
                break;
            }
            default:
                str[written++] = '%';
                str[written++] = *format;
        }
        format++;
    }
    
    str[written] = '\0';
    va_end(args);
    return written;
}

}
