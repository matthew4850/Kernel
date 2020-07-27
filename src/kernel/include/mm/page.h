#ifndef _PAGE_H_
#define _PAGE_H_

#include <paging.h>

// Todo: This should be dynamic at runtime to support different sized pages
#define PAGE_OFFSET_MASK (PAGE_SIZE-1u)
#define PAGE_MASK (~PAGE_OFFSET_MASK)
#define PAGE_ALIGN(addr) (((addr)+PAGE_OFFSET_MASK)&PAGE_MASK)

#endif //_PAGE_H_
