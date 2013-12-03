//
//  mutex.c
//  
//
//  Created by Peller Patrizia on 03/12/2013.
//
//

#include "mutex.h"
#include "malloc.h"
#include "sem.h"

/*static enum _mutex_state_t {
    LOCKED,
    UNLOCKED
}*/

static struct _mutex_s {
    struct pcb_s * owner;
    struct sem_s sem;
}

int mutex_create(int * desc) {
    /* Vérification des paramètres */
    if (desc == NULL) {
        return -1;
    }
    
    /*Création du mutex*/
    struct _mutex_s * mutex = malloc_alloc(sizeof(struct _mutex_s));
    if (mutex == NULL) {
        return -1;
    }
    
    /*Initialisation du mutex*/
    sem_init(&mutex->sem, 1);
    mutex->owner = NULL;
    *desc = (int) mutex;
    
    return 0;
}

/**
 * Méthode privée pour changer le mutex. Renvoye -1 si desc est 0, et à si le changement s'est bien passé.
 */
static int _mutex_desc_convert(int desc, _mutex_s * mutex) {
    if (desc <= 0) {
        return -1;
    }
    *mutex = *(_mutex_s *) desc;
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

int mutex_acquire(int desc) {
    //convertir le desc en mutex
    struct _mutex_s * mutex = NULL;
    if (_mutex_desc_convert(desc, mutex) == -1) {
        return -1;
    }
    
    //sem down
    sem_down(mutex->sem);
    
    //stocker le pcb courant dans le mutex
    mutex->owner = current_process;
    
    return 0;
}

int mutex_release(int desc) {

}
