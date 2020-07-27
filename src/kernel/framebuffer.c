#include <stdint.h>
#include <string.h>
#include "include/framebuffer.h"
#include <paging.h>

void* FrameBuffer;
uint8_t Bpp;
uint32_t Pitch;
uint8_t PixelWidth;
uint16_t screen_width;
uint16_t screen_height;
uint8_t MaskSize;
uint8_t BitMask;

void Initialize(uint64_t addr, uint8_t bpp, uint32_t pitch, uint32_t width, uint32_t height, uint8_t mask_size){
	FrameBuffer = (void *)addr;
	Bpp = bpp;
	Pitch = pitch;
	screen_width = width;
	screen_height = height;
	MaskSize = mask_size;
	BitMask = (uint8_t)((1u << MaskSize) - 1);
	switch (Bpp)
	{
		case 8:
			PixelWidth = 1;
			break;
		case 15:
		case 16:
			PixelWidth = 2;
			break;
		case 24:
			PixelWidth = 3;
			break;

		case 32:
			PixelWidth = 4;
			break;
	}
}

void PutPixelAddr(void* addr, uint32_t color){
	switch (Bpp)
	{
		case 8:
			*(uint8_t *)addr = (uint8_t)color;
			break;
		case 15:
		case 16:
			*(uint16_t *)addr = (uint16_t)color;
			break;
		case 24:
			*(uint32_t *)addr = (color & 0xffffffu) | (*(uint32_t *)addr & 0xff000000);
			break;

		case 32:
			*(uint32_t *)addr = color;
			break;
	}
}

void PutPixel(int x, int y, struct Color color){
	void* pixel = FrameBuffer + Pitch * y + PixelWidth * x;
	uint32_t newColor = ((color.r & BitMask) << (MaskSize * 2u)) | ((color.g & BitMask) << MaskSize)| (color.b & BitMask);
	PutPixelAddr(pixel, newColor);
}
void FillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, struct Color color) {
	uint32_t newColor = ((color.r & BitMask) << (MaskSize * 2u)) | ((color.g & BitMask) << MaskSize)| (color.b & BitMask);

	uint16_t width = x2-x1;
	uint16_t height = y2-y1;

	void* where = FrameBuffer + y1 * Pitch + x1 * PixelWidth;

	uint32_t vincrement = ((Pitch)-(PixelWidth * width));
	for (uint16_t i = 0; i < height; i++) {
		for (uint16_t j = 0; j < width; j++) {
			PutPixelAddr(where, newColor);
			where += PixelWidth;
		}
		where += vincrement;
	}
}

void move_screen_vertical(int16_t shift_amount){
	if(shift_amount == 0){
		return;
	}
	if(shift_amount > 0) {
		memcpy(FrameBuffer, FrameBuffer + shift_amount * Pitch,
		       (screen_height - shift_amount) * Pitch + screen_width * PixelWidth);
	}
	else{
		//TODO: this
	}
}

extern page_directory_t kernel_directory;
void ReInitialize() {
	for(uint32_t i = 0; i < 0x400000; i+= 0x1000) {
		force_page_phys(get_page(FrameBuffer + i, &kernel_directory, true), 0, 1, FrameBuffer + i);
	}
}