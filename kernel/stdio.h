#ifndef STDIO_H
#define STDIO_H

#include <stddef.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

int snprintf(char* str, size_t size, const char* format, ...);
int vsnprintf(char* str, size_t size, const char* format, va_list ap);

#ifdef __cplusplus
}
#endif

#endif /* STDIO_H */
