#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

#include <stdint.h>
#define IDT_FLAG_RESERVED 0x0E

struct idt_flags_struct {
	uint8_t reserved:5;
	uint8_t dpl:2;
	uint8_t p:1;
} __attribute__((packed));
typedef struct idt_flags_struct idt_flags_t;

/**
 * A struct describing an interrupt gate.
 */
struct idt_entry_struct
{
	uint16_t base_lo;       ///< The lower 16 bits of the address to jump to when this interrupt fires.
	uint16_t selector;      ///< Kernel segment selector.
	uint8_t  always0;       ///< This must always be zero.
	idt_flags_t  flags;     ///< More flags. See documentation.
	uint16_t base_hi;       ///< The upper 16 bits of the address to jump to.
} __attribute__((packed));
typedef struct idt_entry_struct idt_entry_t;

/**
 * A struct describing a pointer to an array of interrupt handlers.
 * This is in a format suitable for giving to 'lidt'.
 */
struct idt_ptr_struct
{
	uint16_t limit;
	uint32_t base;  	///< The address of the first element in our idt_entry_t array.
} __attribute__((packed));
typedef struct idt_ptr_struct idt_ptr_t;

typedef struct registers
{
	uint32_t ds;                  ///< Data segment selector
	uint32_t edi, esi, ebp, useless_value, ebx, edx, ecx, eax; ///< Pushed by pusha.
	uint32_t int_no, err_code;    ///< Interrupt number and error code (if applicable)
	uint32_t eip, cs, eflags, esp, ss; ///< Pushed by the processor automatically.
} registers_t;

void init_idt();

// These extern directives let us access the addresses of our ASM ISR handlers.

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void isr128();

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

#define PIC_MASTER 0x20u  			///< IO base address for master PIC
#define PIC_SLAVE 0xA0u				///< IO base address for slave PIC
#define PIC_MASTER_COMMAND PIC_MASTER
#define PIC_MASTER_DATA (PIC_MASTER+1u)
#define PIC_SLAVE_COMMAND PIC_SLAVE
#define PIC_SLAVE_DATA (PIC_SLAVE+1u)

#define PIC_EOI 0x20u

#define ICW1_ICW4 0x01u 		///< ICW4 (not) needed
#define ICW1_SINGLE	0x02u		///< Single (cascade) mode
#define ICW1_INTERVAL4 0x04u	///< Call address interval 4 (8)
#define ICW1_LEVEL 0x08u		///< Level triggered (edge) mode
#define ICW1_INIT 0x10u			///< Initialization - required!

#define ICW4_8086 0x01u			///< 8086/88 (MCS-80/85) mode
#define ICW4_AUTO 0x02u			///< Auto (normal) EOI
#define ICW4_BUF_SLAVE 0x08u	///< Buffered mode/slave
#define ICW4_BUF_MASTER 0x0Cu	///< Buffered mode/master
#define ICW4_SFNM 0x10u			///< Special fully nested (not)


typedef void (*isr_t)(registers_t*);
void register_interrupt_handler(uint8_t n, isr_t handler);

#endif //_INTERRUPTS_H
