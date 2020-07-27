#include <stdbool.h>
#include <mm/phys.h>
#include <mm/page.h>
#include <errno.h>

// This is very inefficient and should be changed
uint32_t free_blocks[0x100000];
uint32_t free_blocks_size = 0;

// This is very inefficient and should be changed
uint32_t reserved_blocks[0x19000];
uint32_t reserved_blocks_size = 0;

bool initialised = false;

uint32_t alloc_block(){
	if(free_blocks_size > 0) {
		return free_blocks[--free_blocks_size];
	}
	return -1;
}

void free_block(uint32_t block){
	free_blocks[free_blocks_size++] = block;
}

void initialise_region(uint32_t start, uint32_t size){
	initialised = true;
	for(uint32_t i = (start + PAGE_OFFSET_MASK) / PAGE_SIZE; i < (start + size) / PAGE_SIZE; i++) {
		for(uint32_t j = 0; j < reserved_blocks_size; j++){
			if(reserved_blocks[j] == i){
				goto cont;
			}
		}
		free_blocks[free_blocks_size++] = i;
		cont:
		continue;
	}
}

int reserve_region(void* start, uint32_t size){
	if (initialised) return EPERM;
	for(uint32_t i = start / PAGE_SIZE; i < (start + size) / PAGE_SIZE; i++) {
		reserved_blocks[reserved_blocks_size++] = i;
	}
	return 0;
}
