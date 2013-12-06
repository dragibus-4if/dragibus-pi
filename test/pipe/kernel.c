#include "../../src/pipe.h"
#include "../../src/malloc.h"
#include "../../src/hw.h"

/* TODO changer ça un jour (en gardant le 42) */
#define ASSERT(cond) if(!(cond)) { *((char *)0) = 42; }

int start_kernel(void) {
    /* Initialisation de la RAM */
    malloc_init((void *) HEAP_START);

    pipe_t readable, writable;

    /* Test les cas d'erreurs */
    ASSERT(pipe_create(NULL, NULL) == -1);
    ASSERT(pipe_create(&readable, NULL) == -1);
    ASSERT(pipe_create(NULL, &writable) == -1);
    ASSERT(pipe_read(NULL, NULL, 0) == -1);
    ASSERT(pipe_write(NULL, NULL, 0) == -1);
    ASSERT(pipe_close(NULL) == -1);

    {
        /* Essaye de fermer un pipe non ouvert */
        long int i;
        for(i = -8 ; i < 65537 ; i++) {
            ASSERT(pipe_close((pipe_t)i) == -1);
        }
    }

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
            char buffer1[8] = {0, 1, 2, 3, 4, 5, 6, 7};
            char buffer2[16] = {0};

            /* Ecriture du buffer1 dans le pipe */
            ssize_t nb_write = pipe_write(writable, buffer1, 8);

            /* Lecture dans le pipe comme un porc (mais tout va bien) */
            ssize_t nb_read = pipe_read(readable, buffer2, 4096);

            /* Quelques tests de base */
            ASSERT(nb_write != -1);
            ASSERT(nb_read != -1);
            ASSERT(nb_read <= 10);

            /* Normalement dans un cas monoproc, l'écriture et la lecture ne
             * pose pas de problème. */
            ASSERT(nb_write == 8);
            ASSERT(nb_read == 8);

            /* Vérification de l'égalité des buffers */
            for(i = 0 ; i < nb_read ; i++) {
                ASSERT(buffer1[i] == buffer2[i]);
            }
        }

        {
            int i;
            char buffer1[1024];
            char buffer2[1024];

            /* Remplissage les buffer (OSEF des valeurs) */
            for(i = 0 ; i < 1024 ; i++) {
                buffer1[i] = (char)i % 100;
                buffer2[i] = (char)(i + 3) % 42;
            }

            /* Ecriture dans le pipe petit bout par petit bout */
            ssize_t nb_write = 0;
            for(i = 0 ; i < 128 ; i++) {
                ssize_t nb = pipe_write(writable, buffer1 + (i * 8), 8);
                ASSERT(nb != -1);
                ASSERT(nb <= 8);
                nb_write += nb;
            }

            /* Lecture dans le pipe d'un coup */
            ssize_t nb_read = pipe_read(readable, buffer2, 1024);
            ASSERT(nb_read != 1);
            ASSERT(nb_read <= nb_write);

            /* Normalement dans un cas monoproc, l'écriture et la lecture ne
             * pose pas de problème. */
            ASSERT(nb_write == 1024);
            ASSERT(nb_read == 1024);

            /* On ne peut pas lire dans le pipe (plus rien à lire) */
            ASSERT(pipe_read(readable, buffer2, 1024) == 0);

            /* Vérification de l'égalité des buffers */
            for(i = 0 ; i < nb_read ; i++) {
                ASSERT(buffer1[i] == buffer2[i]);
            }
        }

        ASSERT(pipe_close(readable) == 0);
        ASSERT(pipe_close(writable) == 0);

        /* On ne peut pas fermer 2 fois les pipes */
        ASSERT(pipe_close(readable) == -1);
        ASSERT(pipe_close(writable) == -1);
    }

    pipe_create(&readable, &writable);

    /* Essaye de créé trop de pipes (limite à 32767 pipes) */
    int i;
    pipe_t pipes[MAX_PIPE];
    for(i = 0 ; i < MAX_PIPE ; i += 2) {
        ASSERT(pipe_create(&pipes[i], &pipes[i + 1]) != -1);
    }
    pipe_t p1, p2;
    ASSERT(pipe_create(&p1, &p2) == -1);

    return 0;
}

/* vim: set ft=c et sw=4 sts=4 */
