#include "pipe.h"
#include "malloc.h"

#define ASSERT(cond) if(!(cond)) { *((char *)0) = 42; }

int main(void) {
    /* Initialisation de la RAM */
    char mem[0x30000 - 0x8000];
    malloc_init((void * )&mem);
    malloc_free((void *)malloc_alloc(10000));

    intptr_t in, out;

    ASSERT(pipe_create(NULL, NULL) == -1);
    ASSERT(pipe_create(&in, NULL) == -1);
    ASSERT(pipe_create(NULL, &out) == -1);

    if(pipe_create(&in, &out) == 0) {
        pipe_close(in);
        pipe_close(out);
    }
    else
      return -1;
    return 0;
}

/* vim: set ft=c et sw=4 sts=4 */
