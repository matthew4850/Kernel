#include "PIT.h"
#include <sys/io.h>
#include <interrupts.h>
#include <sched.h>

uint64_t ticks_since_boot = 0;

static void timer_callback(registers_t* regs)
{
	ticks_since_boot += 1;
	schedule();
	//switch_to_next_task();
	//printk("Tick: %u\n", tick);
}

void init_timer(uint32_t frequency)
{
	register_interrupt_handler(IRQ0, &timer_callback);

	uint32_t divisor = 1193180 / frequency;

	pit_command_t command = {
		.channel = CHANNEL0,
		.access_mode = ACCESS_LOHIBYTE,
		.operating_mode = MODE_SQUARE_WAVE_GENERATOR,
		.BCD = 0
	};

	outb(command.command, 0x43);

	uint8_t lo = divisor & 0xFFu;
	uint8_t hi = (divisor >> 8u) & 0xFFu;

	outb(lo, 0x40);
	outb(hi, 0x40);
}

/*
uint16_t read_PIT_count(void) {
	outb(0, 0x43);
	uint8_t lo = inb(0x40);
	uint8_t hi = inb(0x40);
	return (hi << 8u) | lo;
}*/
