#include <paging.h>
#include <mm/phys.h>
#include <string.h>
#include <panic.h>
#include <tty.h>
#include <mm/page.h>
#include <kmalloc.h>
#include <sched.h>

page_directory_entry_t kernel_page_tables_physical[1024]  __attribute__((aligned(4096))); ///< Physical location of page tables

page_table_t* kernel_page_tables[1024]; ///< Page tables

page_directory_t kernel_directory = {
	.page_tables_physical = &kernel_page_tables_physical,
	.page_tables = &kernel_page_tables,
	.page_directory_physical_address = (uint32_t)&kernel_page_tables_physical - KERNEL_SPACE
};

extern task_t* current_task;

extern uint32_t kernel_start;
extern uint32_t kernel_end;

uint32_t placement_address = (uint32_t)&kernel_end;

#define NUM_PAGES_TO_CREATE 1024 // Todo: allow for dynamic number of free pages at runtime

struct {
	void* phys_address;
	void* virt_address;
} free_page_list[NUM_PAGES_TO_CREATE];

uint32_t num_free_pages;

void alloc_page(page_t* page, int is_kernel, int is_writeable)
{
	if (page->address != 0)
	{
		return;
	}
	else
	{
		uint32_t idx = alloc_block();
		if (idx == (uint32_t)-1)
		{
			PANIC("No free frames!\n");
		}
		page->present = 1;
		page->rw = is_writeable ? 1 : 0;
		page->user = is_kernel ? 0 : 1;
		page->address = idx;
	}
}

void force_page_phys(page_t* page, int is_kernel, int is_writeable, uint32_t addr){
	page->present = 1;
	page->rw = is_writeable ? 1 : 0;
	page->user = is_kernel ? 0 : 1;
	page->address = addr >> 12u;
}

void free_page_entry(page_t* page)
{
	uint32_t frame;
	if (!(frame=page->address))
	{
		return;
	}
	else
	{
		free_block(frame);
		page->address = 0x0;
	}
}

void free_page(uint32_t page_addr)
{
	if(page_addr < KERNEL_SPACE){
		PANIC("Cannot free page in user space")
	}

	page_t* pte = get_page(page_addr, current_task->page_directory, 0);
	if(pte != NULL)
	{
		free_page_list[++num_free_pages].phys_address = (void*)(pte->address << 12u);
		free_page_list[++num_free_pages].virt_address = (void*)page_addr;
	}
	else
		{
		PANIC("Trying to free page but PTE does not exist")
	}
}

extern uint32_t kheap_location;

void initialise_paging()
{
	placement_address = PAGE_ALIGN(placement_address);

	for(uint32_t i = 0; i < NUM_PAGES_TO_CREATE; i++)
	{
		uint32_t page_address = placement_address + i * 0x1000;
		free_page_list[NUM_PAGES_TO_CREATE - 1 - i].phys_address = page_address - KERNEL_SPACE;
		free_page_list[NUM_PAGES_TO_CREATE - 1 - i].virt_address = page_address;
	}

	placement_address += NUM_PAGES_TO_CREATE * PAGE_SIZE; // create 16 free pages
	num_free_pages = NUM_PAGES_TO_CREATE;

	for(uint32_t i = (uint32_t)&kernel_start; i <= placement_address; i+= 0x1000) {
		force_page_phys(get_page(i, &kernel_directory, true), 0, 1, i - KERNEL_SPACE);
	}

	for (uint32_t i = PAGE_ALIGN(placement_address); i < PAGE_ALIGN(placement_address) + KHEAP_SIZE; i += 0x1000)
	{
		alloc_page(get_page(i, &kernel_directory, 1), 1, 1);
	}

	kheap_location = placement_address;

	placement_address += KHEAP_SIZE;

	register_interrupt_handler(14, page_fault);

	switch_page_directory(kernel_directory.page_directory_physical_address);
}

inline void switch_page_directory(uint32_t dir)
{
	asm volatile("mov %0, %%cr3"::"r"(dir));
}

static inline void flush_tlb_entry(void* addr)
{
	asm volatile ("invlpg (%0)" : : "b"(addr) : "memory");
}

void* get_free_page(uint32_t* phys)
{
	if(num_free_pages > 0)
	{
		num_free_pages--;
		if(phys != NULL)
		{
			*phys = (uint32_t)free_page_list[num_free_pages].phys_address;
		}
		return free_page_list[num_free_pages].virt_address;
	}

	PANIC("No free frames left")

	return NULL;
}

page_table_t* get_page_table(uint32_t address, page_directory_t *dir, bool make)
{
	uint32_t table_index = (address / PAGE_SIZE) / PAGE_TABLE_NUM_PAGES;
	if ((*dir->page_tables)[table_index])
	{
		return (*dir->page_tables)[table_index];
	}
	else if(make){
		uint32_t phys_addr;
		page_table_t* page_addr = get_free_page(&phys_addr);
		(*dir->page_tables)[table_index] = page_addr;

		memset(page_addr, 0, 0x1000);

		(*dir->page_tables_physical)[table_index] = (page_directory_entry_t){
			.present = 1,
			.rw = 1,
			.user = 1,
			.address = phys_addr >> 12u
		};
		return (*dir->page_tables)[table_index];
	}
	return NULL;
}

page_t* get_page(uint32_t address, page_directory_t *dir, bool make)
{
	page_table_t* page_table = get_page_table(address, dir, make);

	if (page_table != NULL)
	{
		return &page_table->pages[(address / PAGE_SIZE) % PAGE_TABLE_NUM_PAGES];
	}
	else
	{
		return NULL;
	}
}
void page_fault(registers_t* regs)
{
	uint32_t faulting_address;
	//Get faulting address
	asm ("mov %%cr2, %0" : "=r" (faulting_address));

	pagefault_error_code_t fault = *(pagefault_error_code_t*)&regs->err_code;

	printk("Page fault! ( ");
	if (!fault.present)
	{
		printk("present ");
	}
	if (fault.rw)
	{
		printk("read-only ");
	}
	if (fault.id)
	{
		printk("instruction ");
	}
	if (fault.user)
	{
		printk("user-mode ");
	}
	if (fault.reserved)
	{
		printk("reserved ");
	}
	printk(") at 0x%x\n", faulting_address);
	PANIC("Page Fault")
}

void copy_page_physical(void* src, void* dst)
{
	uint32_t flags;

	asm volatile("pushf; pop %0" : "=rm" (flags): : "memory");
	asm volatile("cli");

	void* src_virt = NULL;
	void* dst_virt = NULL;

	for (size_t i = KERNEL_PAGE_TABLES_START; i < NUM_PAGE_TABLES; i++) {
		if(!(*current_task->page_directory->page_tables_physical)[i].present)
		{
			continue;
		}
		for (size_t j = 0; j < PAGE_TABLE_NUM_PAGES; j++) {
			if(!(*current_task->page_directory->page_tables)[i]->pages[j].present)
			{
				if(src_virt == NULL){
					force_page_phys(&(*current_task->page_directory->page_tables)[i]->pages[j], 1, 1, src);
					src_virt = (void*)((i * (PAGE_TABLE_NUM_PAGES * PAGE_SIZE)) + (j * PAGE_SIZE));
				}
				else{
					force_page_phys(&(*current_task->page_directory->page_tables)[i]->pages[j], 1, 1, dst);
					dst_virt = (void*)((i * (PAGE_TABLE_NUM_PAGES * PAGE_SIZE)) + (j * PAGE_SIZE));
					goto loop_end;
				}
			}
		}
	}
	loop_end:

	if(src_virt == NULL || dst_virt == NULL)
	{
		PANIC("Could not find free virtual address in kernel space")
	}

	flush_tlb_entry(src_virt);
	flush_tlb_entry(dst_virt);

	memcpy(dst_virt, src_virt, PAGE_SIZE);

	page_t* pte = get_page(src_virt, current_task->page_directory, 0);
	pte->present = 0;
	pte = get_page(dst_virt, current_task->page_directory, 0);
	pte->present = 0;

	flush_tlb_entry(src_virt);
	flush_tlb_entry(dst_virt);

	if(flags & 0x0200u)
	{
		// Re enable interrupts if were already enabled
		asm volatile("sti");
	}
}

static page_table_t* clone_table(page_table_t *src, uint32_t *physAddr)
{

	page_table_t *table = (page_table_t*)get_free_page(physAddr);
	memset(table, 0, PAGE_SIZE);

	for (int i = 0; i < 1024; i++)
	{
		if (!src->pages[i].address) {
			continue;
		}

		uint32_t block = alloc_block();
		if (block == (uint32_t)-1)
		{
			PANIC("No free frames!\n");
		}

		table->pages[i] = (page_t){
			.present = src->pages[i].present,
			.rw = src->pages[i].rw,
			.user = src->pages[i].user,
			.accessed = 0,
			.dirty = 0,
			.address = block
		};

		copy_page_physical(src->pages[i].address * PAGE_SIZE, block * PAGE_SIZE);
	}
	return table;
}

void clone_page_directory(page_directory_t* src, page_directory_t* dst)
{
	for (size_t i = 0; i < 1024; i++) {
		if (!(*src->page_tables)[i]) {
			continue;
		}

		if (i >= KERNEL_PAGE_TABLES_START)
		{
			(*dst->page_tables)[i] = (*src->page_tables)[i];
			(*dst->page_tables_physical)[i] = (*src->page_tables_physical)[i];
		}
		else
		{
			uint32_t phys;
			(*dst->page_tables)[i] = clone_table((*src->page_tables)[i], &phys);
			page_directory_entry_t tmp = (*src->page_tables_physical)[i];
			tmp.address = phys >> 12u;
			(*dst->page_tables_physical)[i] = tmp;
		}

	}
}