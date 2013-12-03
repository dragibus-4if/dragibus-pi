#include "pipe.h"
#include "malloc.h"

/* TODO changer ça un jour (en gardant le 42) */
#define ASSERT(cond) if(!(cond)) { *((char *)0) = 42; }

int main(void) {
    /* Initialisation de la RAM */
    char mem[0x30000 - 0x8000];
    malloc_init((void * )&mem);

    intptr_t readable, writable;

    ASSERT(pipe_create(NULL, NULL) == -1);
    ASSERT(pipe_create(&readable, NULL) == -1);
    ASSERT(pipe_create(NULL, &writable) == -1);
    ASSERT(pipe_read(NULL, NULL, 0) == -1);
    ASSERT(pipe_write(NULL, NULL, 0) == -1);
    ASSERT(pipe_close(NULL) == -1);

    if(pipe_create(&readable, &writable) == 0) {
        {
            char buffer1[1];
            char buffer2[1];

            ASSERT(pipe_read(readable, NULL, 0) == -1);
            ASSERT(pipe_read(writable, NULL, 0) == -1);
            ASSERT(pipe_read(writable, buffer2, 0) == -1);
            ASSERT(pipe_read(readable, buffer2, 0) == 0);

            ASSERT(pipe_write(writable, NULL, 0) == -1);
            ASSERT(pipe_write(readable, NULL, 0) == -1);
            ASSERT(pipe_write(readable, buffer1, 0) == -1);
            ASSERT(pipe_write(writable, buffer1, 0) == 0);
        }

        {
            int i;
            char buffer1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
            char buffer2[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

            ssize_t nb_write = pipe_write(writable, buffer1, 10);
            ssize_t nb_read = pipe_read(readable, buffer2, 10000);
            ASSERT(nb_write != -1);
            ASSERT(nb_read != -1);
            ASSERT(nb_read <= 10);

            ssize_t nb = nb_write > nb_read ? nb_read : nb_write;
            for(i = 0 ; i < nb ; i++) {
                ASSERT(buffer1[i] == buffer2[i]);
            }
        }

        {
            int i;
            char buffer1[1024] = {0};
            char buffer2[1024] = {0};
            for(i = 0 ; i < 1024 ; i++) {
                buffer1[i] = i % 100;
                buffer2[i] = (i + 3) % 42;
            }

            ssize_t nb_write = 0;
            for(i = 0 ; i < 100 ; i++) {
                ssize_t nb = pipe_write(writable, buffer1 + (i * 10), 10);
                ASSERT(nb != -1);
                ASSERT(nb <= 10);
                nb_write += nb;
            }
            ssize_t nb_read = pipe_write(writable, buffer1 + 1000, 24);
            ASSERT(nb_read != 1);
            ASSERT(nb_read <= nb_write);
            ASSERT(pipe_read(readable, buffer2, 1024) != -1);
            ASSERT(pipe_read(readable, buffer2, 1) == 0);

            ssize_t nb = nb_write > nb_read ? nb_read : nb_write;
            for(i = 0 ; i < nb ; i++) {
                ASSERT(buffer1[i] == buffer2[i]);
            }
        }

        pipe_close(readable);
        pipe_close(writable);
    }
    else
      return -1;
    return 0;
}

/* vim: set ft=c et sw=4 sts=4 */
