#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <framebuffer.h>
#include <tty.h>
#include <font.h>


static uint16_t WIDTH;
static uint16_t HEIGHT;

static uint16_t terminal_row;
static uint16_t terminal_column;
static struct Color terminal_fg_color = COLOR_WHITE;
static struct Color terminal_bg_color = COLOR_BLACK;

static uint16_t FONT_WIDTH = 12;
static uint16_t FONT_HEIGHT = 16;

static bool initialized;

unsigned char history[5000];
int curchar;

void terminal_initialize(void) {
	FillRect(0, 0, screen_width, screen_height, terminal_bg_color);

	WIDTH = screen_width/(uint16_t)FONT_WIDTH;
	HEIGHT = screen_height/(uint16_t)FONT_HEIGHT;
	initialized = true;
	for(int i = 0; i < curchar; i++){
		terminal_putchar(history[i]);
	}
}

void terminal_setcolor(struct Color color) {
	//todo implement
}


void terminal_putentryat(const unsigned char chr, uint16_t x, uint16_t y) {
	const unsigned char* font_char = &console_font_12x16[2*16* chr];
	for (int l = 0; l < FONT_HEIGHT; l++) {
		for (int i = 0; i < 8; i++) {
			if ((font_char[l*2] & (1u << (7u-i)))) {
				PutPixel(i + x, l + y, terminal_fg_color);
			}
			else{
				PutPixel(i + x, l + y, terminal_bg_color);
			}
		}
		for (int i = 0; i < 4; i++) {
			if ((font_char[l*2+1] & (1u << (7u-i)))) {
				PutPixel(i + x + 8, l + y, terminal_fg_color);
			}
			else{
				PutPixel(i + x + 8, l + y, terminal_bg_color);
			}
		}
	}
}


void scroll(){
	move_screen_vertical(FONT_HEIGHT);
	FillRect(0, screen_height - FONT_HEIGHT, screen_width, screen_height, terminal_bg_color);
}
void terminal_putchar(unsigned char c) {
	if (c == '\n') {
		++terminal_row;
		terminal_column = 0;
	} else{
		terminal_putentryat(c, terminal_column * 12, terminal_row * 16);
		if (++terminal_column >= WIDTH) {
			terminal_column = 0;
			++terminal_row;
		}
	}
	if(terminal_row >= HEIGHT){
		--terminal_row;
		scroll();
	}
}

void terminal_write(const unsigned char* data, size_t size) {
	for (size_t i = 0; i < size; i++) {
		history[curchar] = data[i];
		curchar++;
		if(initialized)
			terminal_putchar(data[i]);
	}
}

void terminal_writestring(const unsigned char* data) {
	terminal_write(data, strlen(data));
}