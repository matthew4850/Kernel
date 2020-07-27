#include <interrupts.h>

#ifndef OSDEV_SYSCALL_H
#define OSDEV_SYSCALL_H

#endif //OSDEV_SYSCALL_H
void initialise_syscalls(void);
void syscall_handler(registers_t* regs);