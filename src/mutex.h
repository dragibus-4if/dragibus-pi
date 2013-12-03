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
 * s'est bien finie, -1 si le desc est NULL ou bien l'allocation de la mémoire a échoué.
 */
int mutex_create(int * desc);

/**
 * Détruire le mutex et libérer la mémoire du mutex. Renvoye -1 si le changement du mutex a été
 * un échec, 0 si tout va bien.
 */
int mutex_free(int desc);

/**
 * Bloquer un processus préciser par desc. Renvoye -1 si le mutex n'a pas pu être modifié, 0 si
 * le processus a bien été bloqué.
 */
int mutex_acquire(int desc);

int mutex_release(int desc);

#endif
