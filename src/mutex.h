//
//  mutex.h
//  
//
//  Created by Peller Patrizia on 03/12/2013.
//
//

#ifndef MUTEX_H
#define MUTEX_H

/**
 * Création du mutex, initialize la sémaphore inclue dans le mutex. Renvoye 0 si la création
 * s'est bien finie, -1 si le desc est NULL ou bien l'allocation du mémoire a échoué.
 */
int mutex_create(int * desc);

int mutex_destroy(int desc);

int mutex_acquire(int desc);

int mutex_release(int desc);

#endif
