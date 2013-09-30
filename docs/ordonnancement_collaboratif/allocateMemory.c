/*
 * malloc.c
 *
 * Initially taken from RPi-Kernel
 *
 * Modified by : Kevin Marquet
 */
#include <stdint.h>

#include "allocateMemory.h"

#define true 1
#define false 0

/*
 * Local prototypes
 *
 */

void createHeader(memory_element_t *address, memory_element_t size);
void createInitialHeader(memory_element_t *address);
void concatenateFreeBlocks(memory_element_t *address);

unsigned char heap[2048];

/*
 * Local variables
 *
 */
int isInitialised;
#define MAGICALLOCATED 0x44414D66     // Dynamically Allocated Memory flag - 'DAMf'

/*
 * Allocate 'size' words and return the pointer to the start.
 *
 */
memory_element_t*
AllocateMemory(memory_element_t size)
{
  memory_element_t* headerNextPointer = (memory_element_t*) heap;

  if (0 == size)
    return (memory_element_t*) 0x80000000;	// deliberately cause exception if used

  if(!isInitialised) {
    createInitialHeader(headerNextPointer);
    isInitialised = true;
  }

  int isFound = false;
  while ((0 != *headerNextPointer) && !isFound) {	  // not at end of list or finished
    if (0 == *(headerNextPointer+1)) { 	                  // not allocated
      memory_element_t sizeOfFreeSpace = (*headerNextPointer - (memory_element_t)(headerNextPointer) - 2);
      if (size <= sizeOfFreeSpace) { 	                  // There is space here
	isFound = true;
	if ((size - sizeOfFreeSpace) >= 0x4) {	          // Insert new header: we have more than is needed
	  // correct previous for next header
	  *((memory_element_t*)(*headerNextPointer) - 1) = (memory_element_t)(headerNextPointer + size + 2);
	  createHeader(headerNextPointer, size);
	} 
	break;
      }
    }
    headerNextPointer = (memory_element_t*)(*headerNextPointer);
  }

  if (!isFound) {  // At the last header without finding space
    createHeader(headerNextPointer, size);
  }
  return (headerNextPointer + 2);
}

int
FreeAllocatedMemory(memory_element_t* address)
{
  if (MAGICALLOCATED == *(address - 1)) {
    concatenateFreeBlocks(address);
    *(address - 1) = 0;	
    return true;	
  }
  return false;
}

/*
 * Local methods
 *
 */

// Concatenate memory blocks if adjacent free areas
void
concatenateFreeBlocks(memory_element_t *address)
{
  while (0 == *(((memory_element_t*)*address)) + 1) {
    if(0 == *((memory_element_t*)*address)) {
      *address = 0;
      break;
    } else {
      *address = *((memory_element_t*)(*address));
      *((memory_element_t*)(*((memory_element_t*)(*address)))) = *((memory_element_t*)(*address) - 1);
    }
  }
}

// Create a memory header
void
createHeader(memory_element_t *address, memory_element_t size)
{
  *(address + size + 2)	= (memory_element_t)(address - 1);	   // previous
  *(address + size + 3)	= (memory_element_t)(*address);		   // next
  *(address + size + 4) = 0;                               // allocated
  *address = (memory_element_t)(address + size + 3);		   // prior header
  *(address + 1) = MAGICALLOCATED;			   // prior allocation. 
}

// Create the first header
void
createInitialHeader(memory_element_t *address)
{
  *(address - 1) = 0;
  *address = 0;
  *(address + 1) = 0;
}

/*
 * Copyright (c) 2012 Ischus Limited www.ischus.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
