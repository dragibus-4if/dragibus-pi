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
    struct _mutex_s mutex = malloc_init(&mutex);
    
    sem_init(mutex->sem, desc);
}

int mutex_destroy(int desc) {

}

int mutex_acquire(int desc) {

}

int mutex_release(int desc) {

}
