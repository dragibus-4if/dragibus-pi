#include "pipe.h"
#include "malloc.h"

int main(void) {
    /* Initialisation de la RAM */
    char mem[0x30000 - 0x8000];
    malloc_init((void * )&mem);

    int in, out;
    if(pipe_create(&in, &out) == 0) {
        pipe_close(in);
        pipe_close(out);
    }
    return 0;
}

/* vim: set ft=c et sw=4 sts=4 */
