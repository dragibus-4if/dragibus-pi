#include "mutex.h"
#include "malloc.h"
#include "sem.h"
#include "sched.h"

/* Structure de données privée du mutex */
struct mutex_s {
    struct pcb_s * owner;
    struct sem_s sem;
};

int mutex_create(struct mutex_s * mutex) {
    /* Vérification des paramètres */
    if (mutex == NULL) {
        return -1;
    }

    /* Initialisation */
    /* TODO reorganise pour que l'appel soit plus du genre sem_create ? */
    sem_init(&mutex->sem, 1);
    mutex->owner = NULL;

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
    sem_down(&mutex->sem);
    mutex->owner = get_current_process();
    return 0;
}

int mutex_release(struct mutex_s * mutex) {
    /* Vérification des paramètres */
    if (mutex == NULL || mutex->owner != get_current_process()) {
        return -1;
    }

    sem_up(mutex->sem);
    return 0;
}

/* vim: set ft=c et sw=2 sts=2 */
