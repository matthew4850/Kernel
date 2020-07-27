#ifndef OSDEV_PHYS_H
#define OSDEV_PHYS_H
#include <stdint.h>


uint32_t alloc_block();
void free_block(uint32_t block);
void initialise_region(uint32_t start, uint32_t size);
int reserve_region(void* start, uint32_t size);

#endif //OSDEV_PHYS_H
