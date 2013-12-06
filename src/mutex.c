#include "mutex.h"
#include "malloc.h"
#include "sem.h"
#include "sched.h"

/* Structure de données privée du mutex */
struct mutex_s {
    struct pcb_s * owner;
    struct sem_s sem;
};

int mutex_create(mutex_s * mutex) {
    /* Vérification des paramètres */
    if (mutex == NULL) {
        return -1;
    }

    /* Initialisation */
    /* TODO reorganise pour que l'appel soit plus du genre sem_create ? */
    sem_init(&mutex->sem, 1);
    mutex->owner = NULL;

    /* Retour */
    _mutex_array[mid] = mutex;
    _current_index = mid;
    *desc = mid;

    return 0;
}

int mutex_free(mutex_s * desc) {
    /* Vérification des paramètres */
    if (mutex == NULL) {
        return -1;
    }

    /* TODO sem_destroy(mutex->sem); ? */
    _mutex_array[desc] = NULL;
    return 0;
}

int mutex_acquire(mutex_s * desc) {
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

int mutex_release(mutex_s * desc) {
    /* Vérification des paramètres */
    if (mutex == NULL) {
        return -1;
    }

    /* TODO */
  return 0;
}
