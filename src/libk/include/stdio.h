#ifndef _STDIO_H
#define _STDIO_H 1

#include <stdarg.h>
#include <stddef.h>

int vsprintf(char* buffer, const char* format, va_list vlist);
int vsprintf(char* restrict buffer, const char* restrict format, va_list vlist);
int vsnprintf(char* restrict buffer, size_t bufsz, const char* restrict format, va_list vlist);

#endif