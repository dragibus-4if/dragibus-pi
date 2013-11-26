#include "malloc.h"

struct malloc_entry_t {
  uint32 size;
  char   data[0];
};

struct malloc_heap_t {
  uint32 size;
  uint32 tail;
  struct malloc_entry_t *entries[0];
};

struct malloc_heap_t *malloc_free_list;
char *malloc_head;

struct malloc_entry_t *malloc_get_entry(char *ptr) {
  return (struct malloc_entry_t *)(ptr - sizeof(struct malloc_entry_t));
}

void malloc_free_list_swap(uint32 x, uint32 y);

void malloc_init(void *ptr) {
  uint32 size = 20480;
  malloc_head = ptr;
  malloc_free_list = (struct malloc_heap_t *) malloc_head;
  malloc_head += sizeof(struct malloc_heap_t) + 
    sizeof(struct malloc_entry_t)*size;
  malloc_free_list->size = size;
  malloc_free_list->tail = 0;
}

void malloc_free(void *ptr) {
  struct malloc_entry_t *entry = malloc_get_entry(ptr);
  uint32 curr = malloc_free_list->tail++;
  uint32 parent = curr / 2;
  malloc_free_list->entries[curr] = entry;
  while( curr != 0 ) {
    if ( 
  	  malloc_free_list->entries[curr]->size < 
	  malloc_free_list->entries[parent]->size 
    ) {
      malloc_free_list_swap(parent, curr);
      curr = parent;
    }
    else {
      break;
    }
  }
}

void malloc_free_list_swap(uint32 x, uint32 y) {
  struct malloc_entry_t *tmp;
  tmp = malloc_free_list->entries[x];
  malloc_free_list->entries[x] = malloc_free_list->entries[y];
  malloc_free_list->entries[y] = tmp;
};

uint32 malloc_leq(uint32 left, uint32 right) {
  return malloc_free_list->entries[left]->size <= 
    malloc_free_list->entries[right]->size;
};

uint32 malloc_le(uint32 left, uint32 right) {
  return malloc_free_list->entries[left]->size < 
    malloc_free_list->entries[right]->size;
};

void malloc_free_list_remove(uint32 curr) {
  if ( malloc_free_list->tail == 0 ) {
    return;
  }

  uint32 tail = --malloc_free_list->tail;
  uint32 left  = (curr*2)+1;
  uint32 right = left+1;
  malloc_free_list_swap(curr, tail);

  while(left < tail) {
    if ( malloc_le(left, right)) {
      malloc_free_list_swap(left, curr);
      curr = left;
    }
    else {
      malloc_free_list_swap(right, curr);
      curr = right;
    }
    left  = (curr*2)+1;
    right = left+1;
  }
}

char *malloc_alloc_new_entry(uint32 size) {
  struct malloc_entry_t *entry = (struct malloc_entry_t *)malloc_head;
  malloc_head += size + sizeof(struct malloc_entry_t);
  entry->size = size;
  return entry->data;
}

char* malloc_alloc(uint32 size) {
  // make all sizes 4 byte aligned
  if ( size & 0x3 ) {
    size = size - (size & 0x3) + 4;
  }
  uint32 curr = 0;
  uint32 tail = malloc_free_list->tail;
  char *ptr = 0;
  while( curr < tail && malloc_free_list->entries[curr]->size < size ) {
    curr = (curr*2)+1;
  }
  if ( curr < tail ) {
    malloc_free_list_remove(curr);
    ptr = malloc_free_list->entries[malloc_free_list->tail]->data;
  }
  else {
    ptr = malloc_alloc_new_entry(size);
  }
  return ptr;
}

uint32 malloc_freelist_length() {
  return malloc_free_list->tail;
}

void memcpy(void *src, void *dst, uint32 len) {
  uint32 ilen = len / 4;
  uint32 clen = len % 4;
  uint32 *isrc = (uint32 *)src;
  uint32 *idst = (uint32 *)dst;
  uint8  *csrc = (uint8 *)src;
  uint8  *cdst = (uint8 *)src;
  uint32 i;
  for(i=0; i<ilen; ++i) {
    idst[i] = isrc[i];
  }
  for(i=1; i<=clen; ++i) {
    cdst[len-i] = csrc[len-i];
  }
}

