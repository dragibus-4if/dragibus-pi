#include "mutex.h"
#include "malloc.h"
#include "sem.h"

/* Structure de données privée du mutex */
static struct _mutex_s {
    struct pcb_s * owner;
    struct sem_s sem;
}

/* Permet de convertir le descripteur de mutex en mutex réel. Cas d'erreur :
 * - desc est négatif ou nul : adresse invalide */
static int _mutex_desc_convert(int desc, _mutex_s * mutex) {
    if (desc <= 0) {
        return -1;
    }
    *mutex = *(_mutex_s *) desc;
    return 0;
}

int mutex_create(int * desc) {
    /* Vérification des paramètres */
    if (desc == NULL) {
        return -1;
    }

    /* Création */
    struct _mutex_s * mutex = malloc_alloc(sizeof(struct _mutex_s));
    if (mutex == NULL) {
        return -1;
    }

    /* Initialisation */
    sem_init(&mutex->sem, 1);
    mutex->owner = NULL;

    /* Retour */
    *desc = (int) mutex;
    return 0;
}

int mutex_free(int desc) {
    struct _mutex_s * mutex = NULL;
    if (_mutex_desc_convert(desc, mutex) == -1) {
        return -1;
    }

    malloc_free(mutex->sem);
    malloc_free(mutex);
    return 0;
}

void mutex_acquire(int desc) {
    /* Convertir le descripteur en mutex */
    struct _mutex_s * mutex = NULL;
    if (_mutex_desc_convert(desc, mutex) == -1) {
        return -1;
    }

    /* Processus :
     * - sem_down bloque le process courant si le mutex est déjà bloqué
     * - immédiatement après, on est sûr que le processus courant a
     *   vérouillé le mutex
     */
    sem_down(mutex->sem);
    mutex->owner = current_process;
    return 0;
}

void mutex_release(int desc) {
    /* TODO */
}
