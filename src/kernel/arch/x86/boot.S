#define ASM_FILE 1
#include <multiboot2.h>
#define STACK_SIZE 0x1000  # 4 KiB

.section .multiboot
.align 8
multiboot_header:
     /*  magic */
     .long   MULTIBOOT2_HEADER_MAGIC
     /*  ISA: i386 */
     .long   MULTIBOOT_ARCHITECTURE_I386
     /*  Header length. */
     .long   multiboot_header_end - multiboot_header
     /*  checksum */
     .long   -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT_ARCHITECTURE_I386 + (multiboot_header_end - multiboot_header))
     .align 8
framebuffer_tag_start:
     .short MULTIBOOT_HEADER_TAG_FRAMEBUFFER
     .short MULTIBOOT_HEADER_TAG_OPTIONAL
     .long framebuffer_tag_end - framebuffer_tag_start
     .long 1024
     .long 768
     .long 32
     .align 8
framebuffer_tag_end:
     .short MULTIBOOT_HEADER_TAG_END
     .short 0
     .long 8
multiboot_header_end:

.text

.globl start, _start, stack_bottom
.extern kernel_main
start:
_start:
	movl $(boot_page_tables - 0xC0000000), %edi
	movl $0, %esi
	movl $2047, %ecx

1:
	cmpl $(kernel_start - 0xC0000000), %esi
	jl 2f
#	cmpl $(end - 0xC0000000), %esi
#	jge 3f

	movl %esi, %edx
	orl $0x003, %edx
	movl %edx, (%edi)


2:
	addl $4096, %esi
	addl $4, %edi
	loop 1b

3:
	movl $(boot_page_tables - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 0 * 4
	movl $(boot_page_tables - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 768 * 4

	movl $(boot_page_tables + 0x1000 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 1 * 4
	movl $(boot_page_tables + 0x1000 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 769 * 4

	movl $(boot_page_directory - 0xC0000000), %ecx
	movl %ecx, %cr3

	movl %cr0, %ecx
	orl $0x80010000, %ecx
	movl %ecx, %cr0

	lea 4f, %ecx
	jmp *%ecx

4:

	movl $0, boot_page_directory

	movl %cr3, %ecx
	movl %ecx, %cr3

	mov $stack_top, %esp

    pushl $0
    popf

    pushl %ebx
    pushl %eax

    cli

	call kernel_main

	cli
hang:	hlt
	jmp hang

.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip STACK_SIZE
stack_top:

# Preallocate pages used for paging. Don't hard-code addresses and assume they
# are available, as the bootloader might have loaded its multiboot structures or
# modules there. This lets the bootloader know it must avoid the addresses.
.section .bss, "aw", @nobits
	.align 0x1000
boot_page_directory:
	.skip 0x1000
boot_page_tables:
	.skip 0x2000