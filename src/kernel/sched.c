#include <sched.h>
#include <kmalloc.h>
#include <string.h>
#include <panic.h>
#include <GDT.h>

extern page_directory_t kernel_directory;
extern const uint32_t stack_bottom;

task_t tasks[NUM_TASKS] = {
	[0] = {
		.id = 0,
		.ppid = 0,
		.kstack = (void*)&stack_bottom,
		.ustack = NULL, // This should never run in user mode
		.regs = {0},
		.page_directory = &kernel_directory
	}
};

size_t next_to_execute = 0;
size_t num_tasks = 1;
task_t* current_task = &tasks[0];
bool enabled = false;

int sys_fork()
{
	size_t magic = 0xDEADBEEF;
	uint32_t flags;

	asm volatile("pushf; pop %0" : "=rm" (flags): : "memory");
	asm volatile("cli");


	task_t* parent_task = current_task;

	page_directory_t* new_directory = kmalloc(sizeof(page_directory_t));

	uint32_t phys;
	new_directory->page_tables_physical = get_free_page(&phys);
	memset(new_directory->page_tables_physical, 0, PAGE_SIZE);
	new_directory->page_directory_physical_address = phys;
	new_directory->page_tables = kmalloc(sizeof(page_table_t));
	memset(new_directory->page_tables, 0, sizeof(page_table_t));

	clone_page_directory(current_task->page_directory, new_directory);

	task_t* new_task = &tasks[num_tasks];
	num_tasks++;

	void* page = get_free_page(NULL);
	*new_task = (task_t){
		.id = num_tasks,
		.ppid = current_task->id,
		.ustack = current_task->ustack,
		.kstack = page,
		.regs = {0},
		.page_directory = new_directory,
		.eip = (size_t)&&child_task
	};

	memcpy(page, parent_task->kstack, PAGE_SIZE);
	child_task:

	if(current_task != parent_task)
	{
		ASSERT(magic == 0xDEADBEEF);
		return 0;
	}

	size_t esp;
	asm volatile("mov %%esp, %0" : "=r"(esp));
	size_t ebp;
	asm volatile("mov %%ebp, %0" : "=r"(ebp));

	new_task->esp = esp + (page - parent_task->kstack);
	new_task->ebp = ebp + (page - parent_task->kstack);

	if(flags & 0x0200u)
	{
		// Re enable interrupts if were already enabled
		asm volatile("sti");
	}
	return tasks[num_tasks-1].id;
}

static void switch_task(task_t* task)
{
	size_t esp;
	asm volatile("mov %%esp, %0" : "=r"(esp));
	size_t ebp;
	asm volatile("mov %%ebp, %0" : "=r"(ebp));

	current_task->eip = (size_t)__builtin_return_address (0);
	current_task->esp = esp;
	current_task->ebp = ebp;

	size_t eip = task->eip;
	esp = task->esp;
	ebp = task->ebp;

	current_task = task;

//	current_directory = current_task->page_directory;
	set_kernel_stack(task->esp);
	switch_to_context(eip, esp, ebp, current_task->page_directory->page_directory_physical_address);
}
void switch_to_next_task(void)
{
	if(++next_to_execute >= num_tasks)
	{
		next_to_execute = 0;
	}
	switch_task(&tasks[next_to_execute]);
}
void init_scheduler(void)
{
	enabled = true;
}
void schedule(void)
{
	if(enabled)
	{
		switch_to_next_task();
	}
}