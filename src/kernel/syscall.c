#include <syscall.h>
#include <interrupts.h>
#include <tty.h>
#include <syscalls/sys_write.h>
#include <sched.h>
#include <elf.h>
#include <string.h>

extern task_t* current_task;

bool check_elf_header(Elf32_Ehdr *hdr) {
	if(!hdr) return false;
	return hdr->e_ident[EI_MAG0] == ELFMAG0 && hdr->e_ident[EI_MAG1] == ELFMAG1 && hdr->e_ident[EI_MAG2] == ELFMAG2 && hdr->e_ident[EI_MAG3] == ELFMAG3;
}

bool elf_check_supported(Elf32_Ehdr *hdr) {
	if(!check_elf_header(hdr)) {
		return false;
	}
	if(hdr->e_ident[EI_CLASS] != ELFCLASS32) {
		return false;
	}
	if(hdr->e_ident[EI_DATA] != ELFDATA2LSB) {
		return false;
	}
	if(hdr->e_machine != EM_386) {
		return false;
	}
	if(hdr->e_ident[EI_VERSION] != EV_CURRENT) {
		return false;
	}
	if(hdr->e_type != ET_REL && hdr->e_type != ET_EXEC) {
		return false;
	}
	return true;
}


// TODO: move to appropriate place
void* load_elf_into_mem(Elf32_Ehdr* elf_header)
{
	// TODO: Security checks
	if(elf_check_supported(elf_header))
	{
		for (uintptr_t x = 0; x < (uint32_t)elf_header->e_shentsize * elf_header->e_shnum; x += elf_header->e_shentsize) {
			// Get section header
			Elf32_Shdr* section_header = (Elf32_Shdr*) ((uintptr_t) elf_header + (elf_header->e_shoff + x));
			if (section_header->sh_addr) {
				for (uintptr_t i = 0; i < section_header->sh_size + 0x2000; i += 0x1000) {
					alloc_page(get_page(section_header->sh_addr + i, current_task->page_directory, 1), 0, 1);
				}
				if (section_header->sh_type == SHT_NOBITS) {
					// Zero out bss
					memset((void*) (section_header->sh_addr), 0, section_header->sh_size);
				}
				else {
					// Copy into memory
					memcpy((void*) (section_header->sh_addr), (void*) ((uintptr_t) elf_header + section_header->sh_offset), section_header->sh_size);
				}
			}
		}
	}

	// TODO: better stack placement
	for (uintptr_t stack_pointer = 0x10000000; stack_pointer < 0x10010000; stack_pointer += 0x1000) {
		alloc_page(get_page(stack_pointer, current_task->page_directory, 1), 0, 1);
	}
	return (void*)elf_header->e_entry;
}

void initialise_syscalls(void){
	register_interrupt_handler(0x80, syscall_handler);
}

void syscall_handler(registers_t* regs)
{
	switch (regs->eax){
		case 4:
			regs->eax = sys_write(regs->ebx, (const void *) regs->ecx, regs->edx);
			return;
		case 57:
			sys_fork();
			return;
		default:
			printk("Unknown syscall %x\n", regs->eax);
	}
	regs->eax = 1;
}
