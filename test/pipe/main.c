#include "pipe.h"
#include "malloc.h"

int main(void) {
    /* Initialisation de la RAM */
    char mem[0x30000 - 0x8000];
    malloc_init((void * )&mem);
    malloc_free((void *)malloc_alloc(10000));

    intptr_t in, out;
    if(pipe_create(&in, &out) == 0) {
        pipe_close(in);
        pipe_close(out);
    }
    else
      return -1;
    return 0;
}

/* vim: set ft=c et sw=4 sts=4 */
