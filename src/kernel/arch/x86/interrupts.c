#include <interrupts.h>
#include <string.h>
#include <stdio.h>
#include <sys/io.h>
#include <stdbool.h>
#include <tty.h>

extern void idt_flush(uint32_t);

static void idt_set_gate(uint8_t, uint32_t, uint16_t, idt_flags_t);

idt_entry_t idt_entries[256];
idt_ptr_t idt_ptr;

isr_t interrupt_handlers[256];


void remap_pic(int master_offset, int slave_offset)
{
	uint8_t master_mask = inb(PIC_MASTER_DATA);
	uint8_t slave_mask = inb(PIC_SLAVE_DATA);

	outb(ICW1_INIT | ICW1_ICW4, PIC_MASTER_COMMAND); // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(ICW1_INIT | ICW1_ICW4, PIC_SLAVE_COMMAND);
	io_wait();

	outb(master_offset, PIC_MASTER_DATA); // ICW2: Master PIC vector offset
	io_wait();
	outb(slave_offset, PIC_SLAVE_DATA); // ICW2: Slave PIC vector offset
	io_wait();

	outb(4, PIC_MASTER_DATA); // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	outb(2, PIC_SLAVE_DATA); // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();

	outb(ICW4_8086, PIC_MASTER_DATA);
	io_wait();
	outb(ICW4_8086, PIC_SLAVE_DATA);
	io_wait();

	outb(master_mask, PIC_MASTER_DATA); // restore saved masks.
	outb(slave_mask, PIC_SLAVE_DATA);
}

void init_idt()
{
	idt_ptr.limit = sizeof(idt_entry_t) * 256 -1;
	idt_ptr.base  = (uint32_t)&idt_entries;

	memset(&idt_entries, 0, sizeof(idt_entry_t) * 256);

	// Todo: maybe not all should be accessible from ring 3?
	idt_flags_t flags = {
		.reserved = IDT_FLAG_RESERVED,
		.dpl = 3,
		.p = 1
	};

	idt_set_gate(0, (uint32_t)isr0, 0x08, flags);
	idt_set_gate(1, (uint32_t)isr1, 0x08, flags);
	idt_set_gate(2, (uint32_t)isr2, 0x08, flags);
	idt_set_gate(3, (uint32_t)isr3, 0x08, flags);
	idt_set_gate(4, (uint32_t)isr4, 0x08, flags);
	idt_set_gate(5, (uint32_t)isr5, 0x08, flags);
	idt_set_gate(6, (uint32_t)isr6, 0x08, flags);
	idt_set_gate(7, (uint32_t)isr7, 0x08, flags);
	idt_set_gate(8, (uint32_t)isr8, 0x08, flags);
	idt_set_gate(9, (uint32_t)isr9, 0x08, flags);
	idt_set_gate(10, (uint32_t)isr10, 0x08, flags);
	idt_set_gate(11, (uint32_t)isr11, 0x08, flags);
	idt_set_gate(12, (uint32_t)isr12, 0x08, flags);
	idt_set_gate(13, (uint32_t)isr13, 0x08, flags);
	idt_set_gate(14, (uint32_t)isr14, 0x08, flags);
	idt_set_gate(15, (uint32_t)isr15, 0x08, flags);
	idt_set_gate(16, (uint32_t)isr16, 0x08, flags);
	idt_set_gate(17, (uint32_t)isr17, 0x08, flags);
	idt_set_gate(18, (uint32_t)isr18, 0x08, flags);
	idt_set_gate(19, (uint32_t)isr19, 0x08, flags);
	idt_set_gate(20, (uint32_t)isr20, 0x08, flags);
	idt_set_gate(21, (uint32_t)isr21, 0x08, flags);
	idt_set_gate(22, (uint32_t)isr22, 0x08, flags);
	idt_set_gate(23, (uint32_t)isr23, 0x08, flags);
	idt_set_gate(24, (uint32_t)isr24, 0x08, flags);
	idt_set_gate(25, (uint32_t)isr25, 0x08, flags);
	idt_set_gate(26, (uint32_t)isr26, 0x08, flags);
	idt_set_gate(27, (uint32_t)isr27, 0x08, flags);
	idt_set_gate(28, (uint32_t)isr28, 0x08, flags);
	idt_set_gate(29, (uint32_t)isr29, 0x08, flags);
	idt_set_gate(30, (uint32_t)isr30, 0x08, flags);
	idt_set_gate(31, (uint32_t)isr31, 0x08, flags);

	idt_set_gate(0x80, (uint32_t)isr128, 0x08, flags);

	remap_pic(0x20, 0x28);

	idt_set_gate(IRQ0, (uint32_t)irq0, 0x08, flags);
	idt_set_gate(IRQ1, (uint32_t)irq1, 0x08, flags);
	idt_set_gate(IRQ2, (uint32_t)irq2, 0x08, flags);
	idt_set_gate(IRQ3, (uint32_t)irq3, 0x08, flags);
	idt_set_gate(IRQ4, (uint32_t)irq4, 0x08, flags);
	idt_set_gate(IRQ5, (uint32_t)irq5, 0x08, flags);
	idt_set_gate(IRQ6, (uint32_t)irq6, 0x08, flags);
	idt_set_gate(IRQ7, (uint32_t)irq7, 0x08, flags);
	idt_set_gate(IRQ8, (uint32_t)irq8, 0x08, flags);
	idt_set_gate(IRQ9, (uint32_t)irq9, 0x08, flags);
	idt_set_gate(IRQ10, (uint32_t)irq10, 0x08, flags);
	idt_set_gate(IRQ11, (uint32_t)irq11, 0x08, flags);
	idt_set_gate(IRQ12, (uint32_t)irq12, 0x08, flags);
	idt_set_gate(IRQ13, (uint32_t)irq13, 0x08, flags);
	idt_set_gate(IRQ14, (uint32_t)irq14, 0x08, flags);
	idt_set_gate(IRQ15, (uint32_t)irq15, 0x08, flags);

	idt_flush((uint32_t)&idt_ptr);
	asm("sti");
}

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, idt_flags_t flags)
{
	idt_entries[num] = (idt_entry_t){
		.base_lo = base & 0xFFFFu,
		.base_hi = (base >> 16u) & 0xFFFFu,
		.selector = sel,
		.always0 = 0,
		.flags = flags
	};

}
void isr_handler(registers_t* regs)
{
	if (interrupt_handlers[regs->int_no] != 0)
	{
		isr_t handler = interrupt_handlers[regs->int_no];
		handler(regs);
	}
	else
	{
		printk("Unknown interrupt: %u\n", regs->int_no);
	}
}

void irq_handler(registers_t* regs)
{
	// If this interrupt involved the slave.
	if (regs->int_no >= 40)
	{
		// Send EOI to slave.
		outb(PIC_EOI, PIC_SLAVE_COMMAND);
	}

	// Send EOI to master.
	outb(PIC_EOI, PIC_MASTER_COMMAND);

	if (interrupt_handlers[regs->int_no] != 0)
	{
		isr_t handler = interrupt_handlers[regs->int_no];
		handler(regs);
	}
	else{
		printk("Unknown interrupt: %u\n", regs->int_no);
	}
}

void register_interrupt_handler(uint8_t n, isr_t handler)
{
	interrupt_handlers[n] = handler;
}