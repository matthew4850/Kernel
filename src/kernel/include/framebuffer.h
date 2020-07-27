//
// Created by Matthew on 04/05/2019.
//

#ifndef OSDEV_FRAMEBUFFER_H
#define OSDEV_FRAMEBUFFER_H
struct Color{
	uint8_t r;
	uint8_t g;
	uint8_t b;
};
uint16_t screen_width;
uint16_t screen_height;
void Initialize(uint64_t addr, uint8_t bpp, uint32_t pitch, uint32_t width, uint32_t height, uint8_t mask_size);
void PutPixel(int x, int y, struct Color color);
void FillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, struct Color color);
void move_screen_vertical(int16_t shift_amount);
void ReInitialize();
#define COLOR_BLACK (struct Color){0, 0, 0}
#define COLOR_WHITE (struct Color){255, 255, 255}
#endif //OSDEV_FRAMEBUFFER_H
