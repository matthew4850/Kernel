#include <drivers/ps2/keyboard.h>
#include <interrupts.h>
#include <sys/io.h>
#include <tty.h>

#define KBD_DATA_PORT 0x60

unsigned char keyboard_map[128] = {
		0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
		'9', '0', '-', '=', '\b',	/* Backspace */
		'\t',			/* Tab */
		'q', 'w', 'e', 'r',	/* 19 */
		't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
		0,			/* 29   - Control */
		'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
		'\'', '`',   0,		/* Left shift */
		'\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
		'm', ',', '.', '/',   0,				/* Right shift */
		'*',
		0,	/* Alt */
		' ',	/* Space bar */
		0,	/* Caps lock */
		0,	/* 59 - F1 key ... > */
		0,   0,   0,   0,   0,   0,   0,   0,
		0,	/* < ... F10 */
		0,	/* 69 - Num lock*/
		0,	/* Scroll Lock */
		0,	/* Home key */
		0,	/* Up Arrow */
		0,	/* Page Up */
		'-',
		0,	/* Left Arrow */
		0,
		0,	/* Right Arrow */
		'+',
		0,	/* 79 - End key*/
		0,	/* Down Arrow */
		0,	/* Page Down */
		0,	/* Insert Key */
		0,	/* Delete Key */
		0,   0,   0,
		0,	/* F11 Key */
		0,	/* F12 Key */
		0,	/* All other keys are undefined */
};
int capsLock = 0;
int shiftDown = 0;

static void keyboard_cb() {
	unsigned char scan_code = inb(KBD_DATA_PORT);
	unsigned char keycode = keyboard_map[scan_code];
	if(scan_code & 0x80u){
		// Key was just released.
		if(scan_code == 0xAA || scan_code == 0xB6){
			shiftDown = 0;
		}
	}else{

		if(keycode == 17) {
			shiftDown = 1;
			return;
		} else if(keycode == 174) {
			// Left arrow key
		//	back_pos();
			return;
		} else if(keycode == 175) {
			// Right arrow key
		//	advance_pos();
			return;
		}
		/*
		 * An ASCII Latin lowercase letter and its
		 * uppercase counterpart differ in the 5th
		 * bit.
		 * Example: A = 01000001
		 *          a = 01100001
		 *
		 * To make uppercase, we simply AND a Latin letter
		 * with 0b1101111, or 0xDF.
		 * To toggle the case, we can XOR the letter with
		 * 0b00100000, or 0x20, to toggle the 5th bit.
		 */
		if(shiftDown){
			keycode = keyboard_map[scan_code + 90];
		}

		if(keycode == 0x0A) {
		//	init_prompt();
		}
		else{
			terminal_write(&keycode, 1);
		}
	}

}

void init_keyboard(void) {
	register_interrupt_handler(IRQ1, &keyboard_cb);
}