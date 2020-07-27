#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>

void terminal_initialize(void);
void terminal_putchar(unsigned char c);
void terminal_write(const unsigned char* data, size_t size);
void terminal_writestring(const unsigned char* data);
void printk(const char *fmt, ...);
#endif