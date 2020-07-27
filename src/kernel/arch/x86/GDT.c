#include <GDT.h>
#include <string.h>

static gdt_entry_t create_entry(gdt_access_t);
static gdt_entry_t create_null_entry();

gdt_entry_t gdt_entries[6];
gdt_ptr_t gdt_ptr;
tss_entry_t tss_entry;

void init_gdt()
{
	gdt_ptr.limit = (sizeof(gdt_entry_t) * 6) - 1;
	gdt_ptr.base  = (uint32_t)&gdt_entries;

	gdt_entry_t null_segment = create_null_entry();
	gdt_entry_t kernel_mode_code_segment = create_entry((struct gdt_access){
			.type = GDT_CODE_TYPE_EXEC_READ,
			.dt   = GDT_CODE_AND_DATA_DESCRIPTOR,
			.dpl  = GDT_RING0,
			.p    = GDT_SEGMENT_PRESENT
	}); ///< Code segment
	gdt_entry_t kernel_mode_data_segment = create_entry((struct gdt_access){
			.type = GDT_DATA_TYPE_READ_WRITE,
			.dt   = GDT_CODE_AND_DATA_DESCRIPTOR,
			.dpl  = GDT_RING0,
			.p    = GDT_SEGMENT_PRESENT
	}); ///< Data segment
	gdt_entry_t user_mode_code_segment = create_entry((struct gdt_access){
			.type = GDT_CODE_TYPE_EXEC_READ,
			.dt   = GDT_CODE_AND_DATA_DESCRIPTOR,
			.dpl  = GDT_RING3,
			.p    = GDT_SEGMENT_PRESENT
	}); ///< User mode code segment
	gdt_entry_t user_mode_data_segment = create_entry((struct gdt_access){
			.type = GDT_DATA_TYPE_READ_WRITE,
			.dt   = GDT_CODE_AND_DATA_DESCRIPTOR,
			.dpl  = GDT_RING3,
			.p    = GDT_SEGMENT_PRESENT
	}); ///< User mode data segment


	uint32_t base = (uint32_t) &tss_entry;
	uint32_t limit = base + sizeof(tss_entry_t);

	gdt_entry_t TSS_descriptor = {
		.base_low = (base & 0xFFFFu),
		.base_middle = (base >> 16u) & 0xFFu,
		.base_high = (base >> 24u) & 0xFFu,
		.limit_low = (limit & 0xFFFFu),
		.granularity = {
			.g = GDT_GRANULARITY_1K,
			.d = GDT_OPERAND_SIZE_16,
			.zero = 0,
			.seglen = (limit >> 16u) & 0x0Fu
		},
		.access = {
			.type = GDT_CODE_TYPE_EXEC_ONLY_ACCESSED,
			.dt   = GDT_SYSTEM_DESCRIPTOR,
			.dpl  = GDT_RING3,
			.p    = GDT_SEGMENT_PRESENT
		}
	}; ///< TSS descriptor


	memset(&tss_entry, 0, sizeof(tss_entry));

	tss_entry.ss0  = 0x10;
	tss_entry.esp0 = 0x0;

	tss_entry.cs   = 0x0b;
	tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = 0x13;


	gdt_entries[0] = null_segment;
	gdt_entries[1] = kernel_mode_code_segment;
	gdt_entries[2] = kernel_mode_data_segment;
	gdt_entries[3] = user_mode_code_segment;
	gdt_entries[4] = user_mode_data_segment;
	gdt_entries[5] = TSS_descriptor;

	gdt_flush((uint32_t)&gdt_ptr);
	tss_flush();
}

void set_kernel_stack(uint32_t stack)
{
	tss_entry.esp0 = stack;
}

static gdt_entry_t create_entry(gdt_access_t access)
{
	gdt_entry_t entry = {
		.base_low = (GDT_BASE & 0xFFFFu),
		.base_middle = (GDT_BASE >> 16u) & 0xFFu,
		.base_high = (GDT_BASE >> 24u) & 0xFFu,
		.limit_low = (GDT_LIMIT & 0xFFFFu),
		.granularity = {
			.g = GDT_GRANULARITY_4K,
			.d = GDT_OPERAND_SIZE_32,
			.zero = 0,
			.seglen = GDT_SEGMENT_LENGTH
		},
		.access = access
	};
	return entry;
}

static gdt_entry_t create_null_entry() {

	gdt_entry_t null_entry = (struct gdt_entry_struct){
		.base_low = 0,
		.base_middle = 0,
		.base_high = 0,
		.limit_low = 0,
		.access = (struct gdt_access){
			.p = 0,
			.dpl = 0,
			.dt = 0,
			.type = 0
		},
		.granularity = (struct gdt_granularity){
			.g = 0,
			.d = 0,
			.zero = 0,
			.seglen = 0
		}
	};
	return null_entry;
}