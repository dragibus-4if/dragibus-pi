#include "pipe.h"
#include "malloc.h"

/* Taille (fixée) d'un block buffer */
static const size_t _buffer_block_size = 512;

/* Block buffer: zone mémoire stockée dans une liste chaînée */
static struct _buffer_block_s {
    char * data;
    struct _buffer_block_s * next;
};

/* Buffer: gestion de la liste chaînée de blocks, ainsi que des curseurs de
 * lecture et d'écriture */
static struct _buffer_s {
    struct _buffer_block_s * head;
    struct _buffer_block_s * tail;
    char * wr_cursor;
    char * rd_cursor;
};

/* Création d'un block de buffer vide */
static struct _buffer_block_s * _buffer_block_create() {
    /* Alloc le block initial */
    struct _buffer_block_s * block = (struct _buffer_block_s *) malloc_alloc(
            sizeof(struct _buffer_block_s));
    if (block == NULL) {
        return NULL;
    }

    /* Alloc les données pour le block initial */
    char * data = malloc_alloc(sizeof(void) * _buffer_block_size);
    if (data == NULL) {
        malloc_free(block);
        return NULL;
    }

    /* Initialisation des variables */
    block->data = data;
    block->next = NULL;

    return block;
}

static int _buffer_block_free(struct _buffer_block_s * buffer) {
    /* Vérification des paramètres */
    if (buffer == NULL) {
        return -1;
    }

    /* Désalloc les données */
    malloc_free(buffer->data);
    malloc_free(buffer);
    return 0;
}

/* Création d'un buffer vide (eg. avec un premier block vide) */
static struct _buffer_s * _buffer_create() {
    /* Alloc le buffer */
    struct _buffer_s * buffer = (struct _buffer_s *) malloc_alloc(
            sizeof(struct _buffer_s));
    if (buffer == NULL) {
        return NULL;
    }

    struct _buffer_block_s * block = _buffer_block_create();
    if (block == NULL) {
        malloc_free(buffer);
        return NULL;
    }

    /* Init des valeurs */
    buffer->head = block;
    buffer->tail = block;
    buffer->wr_cursor = block->data;
    buffer->rd_cursor = block->data;

    return buffer;
}

/* Désallouer un buffer */
static int _buffer_free(struct _buffer_s * buffer) {
    /* Vérification des paramètres */
    if (buffer == NULL) {
        return -1;
    }

    /* Parcours et désallocation */
    struct _buffer_block_s * current = buffer->head;
    while (current != NULL) {
        _buffer_block_free(current);
        current = current->next;
    }

    /* Désallocation */
    malloc_free(buffer);
    return 0;
}

/* Lire *size* octets depuis *buffer* dans *data*, renvoyant le nombre d'octets
 * lus, ou -1 si une erreur a eu lieu. */
static ssize_t _buffer_read(struct _buffer_s * buffer,
        char * data, size_t size) {
    /* Vérification des paramètres */
    if (buffer == NULL || data == NULL) {
        return -1;
    }

    ssize_t read_size = 0;

    /* Ecriture */
    while (size > 0 && buffer->rd_cursor != buffer->wr_cursor) {
        /* Copie des données dans le block courant */
        struct _buffer_block_s * block = buffer->head;
        char * end = (char *)((size_t) block->data + _buffer_block_size);
        while(buffer->rd_cursor != end
                && buffer->rd_cursor != buffer->wr_cursor && size > 0) {
            *data = *buffer->rd_cursor;
            data++;
            buffer->rd_cursor++;
            size--;
            read_size++;
        }

        /* Suppression du premier block si on ne l'utilise plus */
        if (buffer->rd_cursor == end) {
            /* Déplace la lecture au prochain block */
            struct _buffer_block_s * new = block->next;
            buffer->head = new;
            buffer->rd_cursor = new->data;

            /* Suppression de l'ancien block */
            _buffer_block_free(block);
        }
    }

    return read_size;
}

/* TODO commentaire */
static ssize_t _buffer_write(struct _buffer_s * buffer,
        const char * data, size_t size) {
    /* Vérification des paramètres */
    if (buffer == NULL || data == NULL) {
        return -1;
    }

    ssize_t write_size = 0;

    /* Ecriture */
    while (size > 0) {
        /* Copie des données dans le block courant */
        struct _buffer_block_s * block = buffer->tail;
        char * end = block->data + _buffer_block_size;
        while(buffer->wr_cursor != end && size > 0) {
            *buffer->wr_cursor = *data;
            data++;
            buffer->wr_cursor++;
            size--;
            write_size++;
        }

        /* Création d'un nouveau block si on arrive à la fin */
        if (buffer->wr_cursor == end) {
            struct _buffer_block_s * new = _buffer_block_create();
            if (new == NULL) {
                return write_size;
            }

            /* Ajout du block créé comme une nouvelle tail */
            buffer->tail->next = new;
            buffer->tail = new;
            buffer->wr_cursor = new->data;
        }
    }

    return write_size;
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
static int _pipe_des_to_end(intptr_t des, struct _pipe_end_s * pipe) {
    if (des <= 0) {
        return -1;
    }
    *pipe = *(struct _pipe_end_s *) des;
    return 0;
}

int pipe_create(intptr_t * in_des, intptr_t * out_des) {
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
    write_end->state = WRITABLE;

    /* Création du point de sortie */
    read_end = (struct _pipe_end_s *) malloc_alloc(sizeof(struct _pipe_end_s));
    if (read_end == NULL) {
        malloc_free(write_end);
        return -1;
    }
    read_end->state = READABLE;

    /* Passage d'un côté à l'autre du pipe */
    read_end->other_side = write_end;
    write_end->other_side = read_end;

    /* Création du buffer commun */
    write_end->buffer = read_end->buffer = _buffer_create();
    if (write_end->buffer == NULL) {
        malloc_free(read_end);
        malloc_free(write_end);
        return -1;
    }

    /* TODO création du mutex du buffer */

    /* Retour par paramètre */
    *in_des = (intptr_t) read_end;
    *out_des = (intptr_t) write_end;
    return 0;
}

int pipe_close(intptr_t des) {
    struct _pipe_end_s pipe_end;
    if (_pipe_des_to_end(des, &pipe_end) == -1) {
        return -1;
    }
    if (pipe_end.other_side == NULL) {
      _buffer_free(pipe_end.buffer);
    } else {
      pipe_end.other_side->other_side = NULL;
    }
    malloc_free((void *) &pipe_end);
    return 0;
}

ssize_t pipe_read(intptr_t des, void * buffer, size_t bufsize) {
    struct _pipe_end_s pipe_end;
    if (_pipe_des_to_end(des, &pipe_end) == -1) {
        return -1;
    }
    if (pipe_end.state != READABLE) {
        return -1;
    }

    /* TODO bloquer le mutex du buffer */
    ssize_t return_value = _buffer_read(pipe_end.buffer, (char *) buffer, bufsize);
    /* TODO libérer le mutex du buffer */
    return return_value;
}

ssize_t pipe_write(intptr_t des, const void * buffer, size_t bufsize) {
    /* Vérification des paramètres */
    struct _pipe_end_s pipe_end;
    if (_pipe_des_to_end(des, &pipe_end) == -1) {
        return -1;
    }
    if (pipe_end.state != WRITABLE) {
        return -1;
    }

    /* TODO bloquer le mutex associé */
    ssize_t return_value =_buffer_write(pipe_end.buffer, buffer, bufsize);
    /* TODO libérer le mutex associé */

    return return_value;
}

/* vim: set ft=c et sw=4 sts=4 */
