#include <stdio.h>
#include <tty.h>
#include "panic.h"

extern void panic(const char* message, const char* file, uint32_t line)
{
	// We encountered a massive problem and have to stop.
	asm volatile("cli"); // Disable interrupts.


	printk("PANIC(%s)at %s : %u\n", message, file, line);
	// Halt by going into an infinite loop.
	while(1){
		asm("hlt");
	}
}

extern void panic_assert(const char* file, uint32_t line, const char* desc)
{
	// An assertion failed, and we have to panic.
	asm volatile("cli"); // Disable interrupts.

	printk("ASSERTION-FAILED(%s)at %s : %u\n", desc, file, line);
	// Halt by going into an infinite loop.
	while(1){
		asm("hlt");
	}
}
