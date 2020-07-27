#ifndef OSDEV_THREAD_H
#define OSDEV_THREAD_H
#include <stddef.h>
#define MAX_FILES			256

struct thread{
	pid_t pid;
	pid_t ppid;
	struct file *files[MAX_FILES];

} typedef thread_t;
#endif //OSDEV_THREAD_H
