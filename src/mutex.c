#include "mutex.h"
#include "malloc.h"
#include "sem.h"
#include "sched.h"

/* Structure de données privée du mutex */
struct _mutex_s {
    struct pcb_s * owner;
    struct sem_s sem;
};

/* Index courant et max du dernier mutex créé */
static mutex_t _current_index = -1;
#define _max_mutex_index 1024

/* Table des mutex créés */
static struct _mutex_s * _mutex_array[_max_mutex_index] = {NULL};

/* Permet de convertir le descripteur de mutex en mutex réel. Cas d'erreur :
 * - desc est négatif ou nul : adresse invalide */
static int _mutex_desc_convert(mutex_t des, struct _mutex_s * mutex) {
    if (des < 0 || des >= _max_mutex_index) {
        return -1;
    }
    if (_mutex_array[des] == NULL) {
        return -1;
    }
    *mutex = *_mutex_array[des];
    return 0;
}

int mutex_create(mutex_t * desc) {
    /* Vérification des paramètres */
    if (desc == NULL) {
        return -1;
    }

    /* Cherche un index valide */
    mutex_t mid;
    for (mid = (_current_index + 1) % _max_mutex_index ;
        mid != _current_index && _mutex_array[mid] != NULL ;
        mid = (mid + 1) % _max_mutex_index);

    /* Si on a pas trouvé un seul descripteur valide */
    if (mid == _current_index) {
      return -1;
    }

    /* Création */
    struct _mutex_s * mutex = (struct _mutex_s *) malloc_alloc(
        sizeof(struct _mutex_s));
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

int mutex_free(mutex_t desc) {
    struct _mutex_s * mutex = NULL;
    if (_mutex_desc_convert(desc, mutex) == -1) {
        return -1;
    }

    /* TODO sem_destroy(mutex->sem); */
    malloc_free((void *) mutex);
    _mutex_array[desc] = NULL;
    return 0;
}

int mutex_acquire(mutex_t desc) {
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
    sem_down(&mutex->sem);
    mutex->owner = get_current_process();
    return 0;
}

int mutex_release(mutex_t desc) {
    /* TODO */
  return 0;
}
