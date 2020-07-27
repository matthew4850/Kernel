#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <multiboot2.h>
#include <framebuffer.h>
#include <tty.h>
#include <interrupts.h>
#include <GDT.h>
#include <drivers/ps2/keyboard.h>
#include <sys/io.h>
#include <paging.h>
#include <panic.h>
#include <syscall.h>
#include <mm/phys.h>
#include <mm/page.h>
#include <kmalloc.h>
#include <string.h>
#include <sched.h>
#include <PIT.h>

extern uint32_t kernel_start;
extern uint32_t kernel_end;

uint32_t tick = 0;

/**
 * Kernel starts with paging enabled mapped to the 3GiB + 1MiB boundary.
 * Due to the use of position dependent code, paging should not be disabled.
 * If disabling paging is required it must be done in asm and must not return
 * to C code without re-enabling it.
 * @param magic Magic multi boot value to ensure kernel was started with a multiboot2 compatible bootloader
 * @param addr Address of multiboot structure
 */
void kernel_main(unsigned long magic, unsigned long addr)
{
	struct multiboot_tag *tag;
	unsigned size;

	if (magic != MULTIBOOT2_BOOTLOADER_MAGIC)
	{
		printk ("Invalid magic number: 0x%x\n", (unsigned) magic);
		PANIC("Not booted using multiboot2!!!")
	}

	if (addr & 7u) {
		printk("Unaligned mbi: 0x%x\n", addr);
		PANIC("Unaligned mbi")
	}

	size = *(unsigned *)(addr);
	printk ("Announced mbi size 0x%x\n", size);
	uint32_t initrd_location;
	uint32_t initrd_end;

	reserve_region(&kernel_start - KERNEL_SPACE, &kernel_end - &kernel_start);

	struct multiboot_tag_framebuffer* tagfb;

	for (tag = (struct multiboot_tag*) (addr + 8); tag->type != MULTIBOOT_TAG_TYPE_END; tag = (struct multiboot_tag*) ((uint8_t*) tag + ((tag->size + 7) & ~7u)))
	{
		printk ("Tag 0x%x, Size 0x%x\n", tag->type, tag->size);
		switch (tag->type)
		{
			case MULTIBOOT_TAG_TYPE_CMDLINE:
				printk ("Command line = %s\n", ((struct multiboot_tag_string*) tag)->string);
				break;
			case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
				printk ("Boot loader name = %s\n", ((struct multiboot_tag_string*) tag)->string);
				break;
			case MULTIBOOT_TAG_TYPE_MODULE:
				initrd_location = ((struct multiboot_tag_module*) tag)->mod_start;
				initrd_end = ((struct multiboot_tag_module*) tag)->mod_end;
				printk ("Module at 0x%x-0x%x. Command line %s\n", initrd_location, initrd_end, ((struct multiboot_tag_module *) tag)->cmdline);
				break;
			case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
			{
				struct multiboot_tag_basic_meminfo* meminfo = (struct multiboot_tag_basic_meminfo*) tag;
				printk("mem_lower = %uKB, mem_upper = %uKB\n", meminfo->mem_lower, meminfo->mem_upper);
				break;
			}
			case MULTIBOOT_TAG_TYPE_BOOTDEV:
			{
				struct multiboot_tag_bootdev* bootdev = (struct multiboot_tag_bootdev*) tag;
				printk ("Boot device 0x%x,%u,%u\n", bootdev->biosdev, (bootdev->slice, bootdev->part));
				break;
			}
			case MULTIBOOT_TAG_TYPE_MMAP:
			{
				multiboot_memory_map_t *mmap;

				printk ("mmap\n");

				struct multiboot_tag_mmap* tag_mmap = (struct multiboot_tag_mmap *) tag;
					for (mmap = tag_mmap->entries; (uint8_t*) mmap < (uint8_t*) tag + tag->size; mmap = (multiboot_memory_map_t *) ((void*) mmap + tag_mmap->entry_size)) {
					printk(" base_addr = 0x%llx, length = 0x%llx, type = 0x%x\n", mmap->addr, mmap->len, (uint32_t) mmap->type);
					switch (mmap->type){
						case MULTIBOOT_MEMORY_AVAILABLE:
							initialise_region(mmap->addr, mmap->len);
							break;
						case MULTIBOOT_MEMORY_RESERVED:
							break;
						case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE:
							break;
						case MULTIBOOT_MEMORY_NVS:
							break;
						case MULTIBOOT_MEMORY_BADRAM:
							break;
					}
				}
			}
				break;
			case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
			{

				tagfb = (struct multiboot_tag_framebuffer *) tag;

				switch (tagfb->common.framebuffer_type)
				{
					case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
						printk("Video Mode: RGA");
						break;
					default:{
						//maybe this will do something, idk how to handle this
						FillRect(0, 0, screen_width, screen_height, (struct Color) {255, 0, 0});
						PANIC("Video Mode: Unknown, idk how this is being displayed");
					}
						break;
				}
				Initialize(tagfb->common.framebuffer_addr, tagfb->common.framebuffer_bpp,
				           tagfb->common.framebuffer_pitch, tagfb->common.framebuffer_width,
				           tagfb->common.framebuffer_height, tagfb->framebuffer_red_mask_size);
				break;
			}

		}
	}
	tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag
	                                + ((tag->size + 7) & ~7u));
	printk ("Total mbi size 0x%x\n", (unsigned) tag - addr);
	printk("Initialising GDT: ");
	init_gdt();
	printk("done\n");
	printk("Setting up interrupts: ");
	init_idt();
	printk("done\n");

	init_timer(10);
	printk("timer: %d\n", read_PIT_count());
	initialise_paging();

	ReInitialize();
	terminal_initialize();

	initialise_syscalls();

	printk("Initialise keyboard\n");
	init_keyboard();

	// TODO: Do this properly
	void* tmp = get_free_page(NULL);
	set_kernel_stack(tmp + PAGE_SIZE);

	init_scheduler();
	sys_fork();

	switch_to_user_mode();

	//execve();

	for(;;) {
		asm("hlt");
	}
}