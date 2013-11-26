#include "pipe.h"

/**
 * Taille max d'un pipe, inspiré des sources de linux.
 */
static const size_t _pipe_max_size = 1048576;

/**
 * État d'un côté d'un pipe (lecture, écriture, fermé).
 */
static enum _pipe_end_state_t {
    READABLE,
    WRITABLE,
    CLOSED
};

static struct _pipe_end_s {
    int des;
    enum _pipe_end_state_t state;

    /**
     * Buffer du pipe, actuellement représenté par une zone mémoire réallouée à
     * chaque écriture/copie.
     */
    void * buffer;
    size_t bufsize;

    /* TODO ajouter le mutex commun du pipe */
};

int pipe_create(int * in_des, int * out_des);

int pipe_close(int des);

ssize_t pipe_read(int * des, void * buffer, size_t bufsize);

ssize_t pipe_write(int * des, const void * buffer, size_t bufsize);
