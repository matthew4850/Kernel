#ifndef _PIT_H_
#define _PIT_H_
#include <stddef.h>
uint16_t read_PIT_count(void);
void set_PIT_count(uint16_t);

#define CHANNEL0 0b00
#define CHANNEL1 0b01
#define CHANNEL2 0b10
#define READ_BACK 0b11 ///< 8254 only

#define ACCESS_LATCH_COUNT 0b00
#define ACCESS_LOBYTE 0b01
#define ACCESS_HIBYTE 0b10
#define ACCESS_LOHIBYTE 0b11

#define MODE_INTERRUPT_ON_TERMINAL_COUNT 0b000
#define MODE_HARDWARE_RETRIGGERABLE_ONE_SHOT 0b001
#define MODE_RATE_GENERATOR 0b010
#define MODE_SQUARE_WAVE_GENERATOR 0b011
#define MODE_SOFTWARE_TRIGGERED_STROBE 0b100
#define MODE_HARDWARE_TRIGGERED_STROBE 0b101
#define MODE_RATE_GENERATOR_2 0b110 ///< Same as MODE_RATE_GENERATOR
#define MODE_SQUARE_WAVE_GENERATOR_2 0b111 ///< Same as MODE_SQUARE_WAVE_GENERATOR

typedef union pit_command{
	struct
	{
		uint8_t BCD : 1; ///< 1 for bcd mode, 0 for binary
		uint8_t operating_mode : 3;
		uint8_t access_mode : 2;
		uint8_t channel : 2;
	};
	uint8_t command;
} pit_command_t;

void init_timer(uint32_t frequency);
#endif //_PIT_H_
