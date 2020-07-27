#include <stdio.h>
#include <tty.h>

#define KPRINTF_BUFSIZ  2048


void printk(const char *fmt, ...) {
	va_list arg;

	va_start(arg, fmt);
	char str[KPRINTF_BUFSIZ];
	int n;

	n = vsnprintf(str, KPRINTF_BUFSIZ, fmt, arg);
	if (n > 0)
		terminal_write(str, n);
		//tty_write(DEV_CONSOLE, str, n);
	va_end(arg);
}