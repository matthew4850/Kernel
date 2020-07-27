#ifndef _SCHED_H_
#define _SCHED_H_

#include <stddef.h>
#include <interrupts.h>
#include <paging.h>

#define NUM_TASKS 128

typedef struct task
{
	pid_t id;
	pid_t ppid;
	void* kstack;
	void* ustack;
	registers_t regs;
	page_directory_t* page_directory;
	size_t esp;
	size_t ebp;
	size_t eip;
} task_t;

int sys_fork();

extern void switch_to_user_mode(void);
void schedule(void);
void init_scheduler(void);
void switch_to_context(size_t eip, size_t esp, size_t ebp, size_t page_dir);

#endif //_SCHED_H_
