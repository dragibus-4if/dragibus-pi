#ifndef ALLOCATEMEMORY_H
#define ALLOCATEMEMORY_H

#include <stdint.h>

typedef uint32_t memory_element_t;

uint32_t* AllocateMemory(uint32_t size);
int FreeAllocatedMemory(uint32_t* address);

#endif
