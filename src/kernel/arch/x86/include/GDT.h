#ifndef OSDEV_GDT_H
#define OSDEV_GDT_H
#include <stdint.h>
#include <stdio.h>

#define GDT_DATA_TYPE_READ_ONLY                       0x0
#define GDT_DATA_TYPE_READ_ONLY_ACCESSED              0x1
#define GDT_DATA_TYPE_READ_WRITE                      0x2
#define GDT_DATA_TYPE_READ_WRITE_ACCESSED             0x3
#define GDT_DATA_TYPE_READ_ONLY_EXPAND_DOWN           0x4
#define GDT_DATA_TYPE_READ_ONLY_EXPAND_DOWN_ACCESSED  0x5
#define GDT_DATA_TYPE_READ_WRITE_EXPAND_DOWN          0x6
#define GDT_DATA_TYPE_READ_WRITE_EXPAND_DOWN_ACCESSED 0x7
#define GDT_CODE_TYPE_EXEC_ONLY                       0x8
#define GDT_CODE_TYPE_EXEC_ONLY_ACCESSED              0x9
#define GDT_CODE_TYPE_EXEC_READ                       0xA
#define GDT_CODE_TYPE_EXEC_READ_ACCESSED              0xB
#define GDT_CODE_TYPE_EXEC_CONFORMING                 0xC
#define GDT_CODE_TYPE_EXEC_CONFORMING_ACCESSED        0xD
#define GDT_CODE_TYPE_EXEC_READ_CONFORMING            0xE
#define GDT_CODE_TYPE_EXEC_READ_CONFORMING_ACCESSED   0xF
// DT field (descriptor type)
#define GDT_SYSTEM_DESCRIPTOR                         0
#define GDT_CODE_AND_DATA_DESCRIPTOR                  1
// DPL field (which ring)
#define GDT_RING0                                     0
#define GDT_RING1                                     1
#define GDT_RING2                                     2
#define GDT_RING3                                     3
// P field (present)
#define GDT_SEGMENT_NOT_PRESENT                       0
#define GDT_SEGMENT_PRESENT                           1

#define GDT_BASE                                      0x00000000u
#define GDT_LIMIT                                     0xFFFFFFFFu
#define GDT_SEGMENT_LENGTH                            (GDT_LIMIT >> 16u) & 0x0Fu

#define GDT_OPERAND_SIZE_16                           0
#define GDT_OPERAND_SIZE_32                           1

#define GDT_GRANULARITY_1K                            0
#define GDT_GRANULARITY_4K                            1

struct gdt_access {
	uint8_t type:4; /// Which type?
	uint8_t dt:1;   ///< Descriptor type
	uint8_t dpl:2;  ///< Ring number (0-3)
	uint8_t p:1;    ///< Is the segment present?
} __attribute__((packed));
typedef struct gdt_access gdt_access_t;

struct gdt_granularity {
	uint8_t seglen:4; ///< Always 0
	uint8_t zero:2;   ///< Operand Size
	uint8_t d:1;      ///< Operand Size
	uint8_t g:1;      ///< Granularity
} __attribute__((packed));
typedef struct gdt_granularity gdt_gran_t;

struct gdt_entry_struct
{
	uint16_t limit_low;           ///< The lower 16 bits of the limit.
	uint16_t base_low;            ///< The lower 16 bits of the base.
	uint8_t  base_middle;         ///< The next 8 bits of the base.
	gdt_access_t  access;         ///< Access flags, determine what ring this segment can be used in.
	gdt_gran_t  granularity;
	uint8_t  base_high;           ///< The last 8 bits of the base.
} __attribute__((packed));
typedef struct gdt_entry_struct gdt_entry_t;

struct gdt_ptr_struct
{
	uint16_t limit;               ///< The upper 16 bits of all selector limits.
	uint32_t base;                ///< The address of the first gdt_entry_t struct.
}
__attribute__((packed));
typedef struct gdt_ptr_struct gdt_ptr_t;

struct tss_entry_struct
{
	uint32_t prev_tss;   ///< The previous TSS, with hardware contenxt switching this would create a linked list
	uint32_t esp0;       ///< The stack pointer to load when we change to kernel mode.
	uint32_t ss0;        ///< The stack segment to load when we change to kernel mode.
	uint32_t esp1;       ///< Unused due to software context switching
	uint32_t ss1;        ///< Unused due to software context switching
	uint32_t esp2;       ///< Unused due to software context switching
	uint32_t ss2;        ///< Unused due to software context switching
	uint32_t cr3;        ///< Unused due to software context switching
	uint32_t eip;        ///< Unused due to software context switching
	uint32_t eflags;     ///< Unused due to software context switching
	uint32_t eax;        ///< Unused due to software context switching
	uint32_t ecx;        ///< Unused due to software context switching
	uint32_t edx;        ///< Unused due to software context switching
	uint32_t ebx;        ///< Unused due to software context switching
	uint32_t esp;        ///< Unused due to software context switching
	uint32_t ebp;        ///< Unused due to software context switching
	uint32_t esi;        ///< Unused due to software context switching
	uint32_t edi;        ///< Unused due to software context switching
	uint32_t es;         ///< Unused due to software context switching
	uint32_t cs;         ///< Unused due to software context switching
	uint32_t ss;         ///< Unused due to software context switching
	uint32_t ds;         ///< Unused due to software context switching
	uint32_t fs;         ///< Unused due to software context switching
	uint32_t gs;         ///< Unused due to software context switching
	uint32_t ldt;        ///< Unused due to software context switching
	uint16_t trap;       ///< Unused due to software context switching
	uint16_t iomap_base; ///< Unused due to software context switching
} __attribute__((packed));

typedef struct tss_entry_struct tss_entry_t;

void init_gdt(void);
void set_kernel_stack(uint32_t stack);

extern void gdt_flush(uint32_t);
extern void tss_flush();


#endif //OSDEV_GDT_H
