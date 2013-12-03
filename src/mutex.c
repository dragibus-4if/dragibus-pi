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
    struct sem_s * sem;
}

int mutex_create(int * desc) {
    /* Vérification des paramètres */
    if (desc == NULL) {
        return -1;
    }
    
    /*Création du mutex*/
    struct _mutex_s * mutex = malloc_alloc(sizeof(struct _mutex_s));
    
    /*Initialisation du mutex*/
    sem_init(&mutex->sem, 1);
    
    
}

int mutex_destroy(int desc) {

}

int mutex_acquire(int desc) {

}

int mutex_release(int desc) {

}
