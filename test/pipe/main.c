#include "pipe.h"
#include "malloc.h"

/* TODO changer ça un jour (en gardant le 42) */
#define ASSERT(cond) if(!(cond)) { *((char *)0) = 42; }

int main(void) {
    /* Initialisation de la RAM */
    char mem[0x30000 - 0x8000];
    malloc_init((void * )&mem);

    intptr_t readable, writable;
    char buffer1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    char buffer2[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    ASSERT(pipe_create(NULL, NULL) == -1);
    ASSERT(pipe_create(&readable, NULL) == -1);
    ASSERT(pipe_create(NULL, &writable) == -1);
    ASSERT(pipe_read(NULL, NULL, 0) == -1);
    ASSERT(pipe_write(NULL, NULL, 0) == -1);
    ASSERT(pipe_close(NULL) == -1);

    if(pipe_create(&readable, &writable) == 0) {
        ASSERT(pipe_read(readable, NULL, 0) == -1);
        ASSERT(pipe_read(writable, NULL, 0) == -1);
        ASSERT(pipe_read(writable, buffer2, 0) == -1);
        ASSERT(pipe_read(readable, buffer2, 0) == 0);

        ASSERT(pipe_write(writable, NULL, 0) == -1);
        ASSERT(pipe_write(readable, NULL, 0) == -1);
        ASSERT(pipe_write(readable, buffer1, 0) == -1);
        ASSERT(pipe_write(writable, buffer1, 0) == 0);

        ssize_t nb_write = pipe_write(writable, buffer1, 10);
        ssize_t nb_read = pipe_read(readable, buffer2, 1000);
        ASSERT(nb_write != -1);
        ASSERT(nb_read != -1);
        ASSERT(nb_read <= 10);

        ssize_t nb = nb_write > nb_read ? nb_read : nb_write;
        int i;
        for(i = 0 ; i < nb ; i++) {
            ASSERT(buffer1[i] == buffer2[i]);
        }

        pipe_close(readable);
        pipe_close(writable);
    }
    else
      return -1;
    return 0;
}

/* vim: set ft=c et sw=4 sts=4 */
