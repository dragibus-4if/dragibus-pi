#include "pipe.h"
#include "malloc.h"

/* TODO commentaire */
static const size_t _buffer_block_size = 512;

/* TODO commentaire */
static struct _buffer_block_s {
    void * data;
    struct _buffer_block_s * next;
};

/* TODO commentaire */
static struct _buffer_s {
    struct _buffer_block_s * head;
    struct _buffer_block_s * tail;
    size_t size;
    void * wr_cursor;
    void * rd_cursor;
};

/* TODO commentaire */
static struct _buffer_s * _buffer_create() {
    /* Alloc le buffer */
    struct _buffer_s * buffer = (struct _buffer_s *) malloc_alloc(
            sizeof(struct _buffer_s));
    if (buffer == NULL) {
        return NULL;
    }

    /* Alloc le block initial */
    struct _buffer_block_s * block = (struct _buffer_block_s *) malloc_alloc(
            sizeof(struct _buffer_block_s));
    if (block == NULL) {
        malloc_free(buffer);
        return NULL;
    }

    /* Alloc les données pour le block initial */
    void * data = malloc_alloc(sizeof(char) * _buffer_block_size);
    if (data == NULL) {
        malloc_free(buffer);
        malloc_free(block);
        return NULL;
    }

    /* Init des valeurs */
    block->data = data;
    block->next = NULL;
    buffer->head = block;
    buffer->tail = block;
    buffer->size = 0;
    buffer->wr_cursor = data;
    buffer->rd_cursor = data;

    return buffer;
}

/* Désallouer un buffer */
static int _buffer_destroy(struct _buffer_s * buffer) {
    /* Vérification des paramètres */
    if (buffer == NULL) {
        return -1;
    }

    /* Parcours et désallocation */
    struct _buffer_block_s current = buffer->head;
    while (current != NULL) {
        struct _buffer_block_s tmp = current;
        current = current->next;
        malloc_free(tmp);
        malloc_free(tmp->data);
    }

    /* Désallocation */
    malloc_free(buffer);
    return 0;
}

/* TODO commentaire */
static ssize_t _buffer_read(struct _buffer_s * buffer,
        void * data, size_t size) {
    /* Vérification des paramètres */
    if (buffer == NULL || data == NULL) {
        return -1;
    }
    return -1;
}

/* TODO commentaire */
static ssize_t _buffer_write(struct _buffer_s * buffer,
        const void * data, size_t size) {
    /* TODO */
    return -1;
}

/**
 * Taille max d'un pipe, inspiré des sources de linux.
 */
static const size_t _pipe_max_size = 1048576;

/**
 * État d'un côté d'un pipe (lecture, écriture, fermé).
 */
static enum _pipe_end_state_t {
    READABLE,
    WRITABLE
};

static struct _pipe_end_s {
    int des;
    enum _pipe_end_state_t state;

    struct _pipe_end_s * other_side;

    /**
     * Buffer du pipe, actuellement représenté par une liste chaînée de blocks
     * mémoire.
     */
    struct _buffer_s * buffer;

    /* TODO ajouter le mutex commun du pipe */
};

/* Permet de faire la liaison entre un descripteur d'extrémité de pipe avec
 * l'élément de la structure correspondant */
static int _pipe_des_to_end(int des, _pipe_end_s * pipe) {
    if (des <= 0) {
        return -1;
    }
    *pipe = *(_pipe_end_s *) des;
    if (pipe->des != des) {
        return -1;
    }
    return 0;
}

int pipe_create(int * in_des, int * out_des) {
    struct _pipe_end_s * read_end;
    struct _pipe_end_s * write_end;

    /* Vérification des paramètres */
    if (in_des == NULL || out_des == NULL) {
        return -1;
    }

    /* Création du point d'entrée */
    write_end = (struct _pipe_end_s *) malloc_alloc(sizeof(struct _pipe_end_s));
    if (write_end == NULL) {
        return -1;
    }
    write_end->des = (int) write_end;
    write_end->state = WRITABLE;

    /* Création du point de sortie */
    read_end = (struct _pipe_end_s *) malloc_alloc(sizeof(struct _pipe_end_s));
    if (read_end == NULL) {
        malloc_free(write_end);
        return -1;
    }
    read_end->des = (int) read_end;
    read_end->state = READABLE;

    /* Passage d'un côté à l'autre du pipe */
    read_end->other_side = write_end;
    write_end->other = read_end;

    /* Création du buffer commun */
    write_end->buffer = read_end->buffer = _buffer_create();
    if (write_end->buffer == NULL) {
        malloc_free(read_end);
        malloc_free(write_end);
        return -1;
    }

    /* Retour par paramètre */
    *in_des = (int) read_end;
    *out_des = (int) write_end;
    return 0;
}

int pipe_close(int des) {
    _pipe_end_s * pipe_end = NULL;
    if (_pipe_des_to_end(des, pipe_end) == -1) {
        return -1;
    }
    if (pipe_end->other_side == NULL) {
      _buffer_destroy(pipe_end->buffer);
    } else {
      pipe_end->other_side->other_side = NULL;
    }
    malloc_free((void *) pipe_end);
    return 0;
}

ssize_t pipe_read(int des, void * buffer, size_t bufsize) {
    _pipe_end_s * pipe_end = NULL;
    if (_pipe_des_to_end(des, pipe_end) == -1) {
        return -1;
    }
    if (pipe_end->state != READABLE) {
        return -1;
    }

    /* TODO bloquer le mutex du buffer */
    ssize_t return_value = _buffer_read(pipe_end->buffer, buffer, bufsize);
    /* TODO libérer le mutex du buffer */
    return return_value;
}

ssize_t pipe_write(int des, const void * buffer, size_t bufsize) {
    /* Vérification des paramètres */
    _pipe_end_s * pipe_end = NULL;
    if (_pipe_des_to_end(des, pipe_end) == -1) {
        return -1;
    }
    if (pipe_end != WRITABLE) {
        return -1;
    }

    /* TODO bloquer le mutex associé */

    if (_buffer_write(pipe_end->buffer, buffer, bufsize) == -1) {
        /* TODO libérer le mutex associé */
        return -1;
    }

    /* TODO libérer le mutex associé */

    return -1;
}

/* vim: set ft=c et sw=4 sts=4 */
