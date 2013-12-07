#include "mutex.h"
#include "malloc.h"
#include "sem.h"
#include "sched.h"

/* Structure de données représentant un mutex */
struct mutex_s {
    struct task_struct * owner;
    struct sem_s sem;
};

struct mutex_s * mutex_create() {
    struct mutex_s * mutex = (struct mutex_s *) malloc_alloc(
            sizeof(struct mutex_s));
    if (mutex == NULL) {
        return NULL;
    }

    if (mutex_init(mutex) == -1) {
        malloc_free(mutex);
        return NULL;
    }
    return mutex;
}

int mutex_free(struct mutex_s * mutex) {
    /* Vérification des paramètres */
    if (mutex == NULL) {
        return -1;
    }

    malloc_free(mutex);
    return 0;
}

int mutex_init(struct mutex_s * mutex) {
    /* Vérification des paramètres */
    if (mutex == NULL) {
        return -1;
    }

    /* Initialisation */
    /* TODO reorganise pour que l'appel soit plus du genre sem_create ? */
    mutex->owner = NULL;
    sem_init(&mutex->sem, 1);

    return 0;
}

int mutex_acquire(struct mutex_s * mutex) {
    /* Vérification des paramètres */
    if (mutex == NULL) {
        return -1;
    }

    /* Processus :
     * - sem_down bloque le process courant si le mutex est déjà bloqué
     * - immédiatement après, on est sûr que le processus courant a
     *   vérouillé le mutex
     */
    /* FIXME fait planter le kernel */
    sem_down(&mutex->sem);
    mutex->owner = get_current_process();
    return 0;
}

int mutex_release(struct mutex_s * mutex) {
    /* Vérification des paramètres */
    if (mutex == NULL || mutex->owner != get_current_process()) {
        return -1;
    }

    mutex->owner = NULL;
    sem_up(&mutex->sem);
    return 0;
}

/* vim: set ft=c et sw=2 sts=2 */
