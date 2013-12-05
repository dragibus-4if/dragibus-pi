#include "pipe.h"
#include "malloc.h"
#include "mutex.h"

/* Taille (fixée) d'un block buffer */
static const size_t _buffer_block_size = 512;

/* Block buffer: zone mémoire stockée dans une liste chaînée */
struct _buffer_block_s {
    char * data;
    struct _buffer_block_s * next;
};

/* Buffer: gestion de la liste chaînée de blocks, ainsi que des curseurs de
 * lecture et d'écriture */
struct _buffer_s {
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
    char * data = malloc_alloc(sizeof(char) * (uint32_t)_buffer_block_size);
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
enum _pipe_end_state_t {
    READABLE,
    WRITABLE
};

struct _pipe_end_s {
    enum _pipe_end_state_t state;

    struct _pipe_end_s * other_side;

    /**
     * Buffer du pipe, actuellement représenté par une liste chaînée de blocks
     * mémoire.
     */
    struct _buffer_s * buffer;

    /* Mutex commun du pipe */
    mutex_t mutex;
};

/* Index courant et max du dernier pipe créé */
static pipe_t _current_index = -1;
#define _max_pipe_index 32767

/* Table des pipes créés */
static struct _pipe_end_s * _pipe_array[_max_pipe_index] = {NULL};

/* Permet de faire la liaison entre un descripteur d'extrémité de pipe avec
 * l'élément de la structure correspondant */
static int _pipe_des_to_end(pipe_t des, struct _pipe_end_s * pipe) {
    if (des < 0 || des >= _max_pipe_index) {
        return -1;
    }
    if (_pipe_array[des] == NULL) {
        return -1;
    }
    *pipe = *_pipe_array[des];
    return 0;
}

int pipe_create(pipe_t * in_des, pipe_t * out_des) {
    /* Vérification des paramètres */
    if (in_des == NULL || out_des == NULL) {
        return -1;
    }

    /* Cherche un premier index valide */
    pipe_t p1;
    for (p1 = (_current_index + 1) % _max_pipe_index ;
        p1 != _current_index && _pipe_array[p1] != NULL ;
        p1 = (p1 + 1) % _max_pipe_index);

    /* Si on a pas trouvé un seul descripteur valide */
    if (p1 == _current_index) {
      return -1;
    }

    /* Cherche un deuxième index valide */
    pipe_t p2;
    for (p2 = (p1 + 1) % _max_pipe_index ;
        p2 != _current_index && _pipe_array[p2] != NULL ;
        p2 = (p2 + 1) % _max_pipe_index);

    /* Si on a pas trouvé un deuxième descripteur valide */
    if (p2 == _current_index) {
      return -1;
    }

    /* Création du point d'entrée */
    struct _pipe_end_s * write_end;
    write_end = (struct _pipe_end_s *) malloc_alloc(sizeof(struct _pipe_end_s));
    if (write_end == NULL) {
        return -1;
    }
    write_end->state = WRITABLE;

    /* Création du point de sortie */
    struct _pipe_end_s * read_end;
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
    struct _buffer_s * buffer = _buffer_create();
    if (buffer == NULL) {
        malloc_free(read_end);
        malloc_free(write_end);
        return -1;
    }
    read_end->buffer = buffer;
    write_end->buffer = buffer;

    /* Création du mutex commun */
    mutex_t mutex = NULL;
    if (mutex_create(&mutex) == -1) {
        _buffer_free(write_end->buffer);
        malloc_free(read_end);
        malloc_free(write_end);
        return -1;
    }
    read_end->mutex = mutex;
    write_end->mutex = mutex;

    /* Retour par paramètre */
    _pipe_array[p1] = read_end;
    _pipe_array[p2] = write_end;
    _current_index = p2;
    *in_des = p1;
    *out_des = p2;

    return 0;
}

int pipe_close(pipe_t des) {
    struct _pipe_end_s pipe_end;
    if (_pipe_des_to_end(des, &pipe_end) == -1) {
        return -1;
    }
    if (pipe_end.other_side == NULL) {
      _buffer_free(pipe_end.buffer);
        mutex_free(pipe_end.mutex);
    } else {
      pipe_end.other_side->other_side = NULL;
    }
    malloc_free((void *) &pipe_end);
    _pipe_array[des] = NULL;
    return 0;
}

ssize_t pipe_read(pipe_t des, void * buffer, size_t bufsize) {
    struct _pipe_end_s pipe_end;
    if (_pipe_des_to_end(des, &pipe_end) == -1) {
        return -1;
    }
    if (pipe_end.state != READABLE) {
        return -1;
    }

    mutex_acquire(pipe_end.mutex);
    ssize_t return_value = _buffer_read(pipe_end.buffer, (char *) buffer, bufsize);
    mutex_release(pipe_end.mutex);
    return return_value;
}

ssize_t pipe_write(pipe_t des, const void * buffer, size_t bufsize) {
    /* Vérification des paramètres */
    struct _pipe_end_s pipe_end;
    if (_pipe_des_to_end(des, &pipe_end) == -1) {
        return -1;
    }
    if (pipe_end.state != WRITABLE) {
        return -1;
    }

    mutex_acquire(pipe_end.mutex);
    ssize_t return_value =_buffer_write(pipe_end.buffer, buffer, bufsize);
    mutex_release(pipe_end.mutex);

    return return_value;
}

/* vim: set ft=c et sw=4 sts=4 */
