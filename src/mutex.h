#ifndef MUTEX_H
#define MUTEX_H

#include "types.h"

extern struct mutex_s;

/* Crée un mutex. Renvoie 0 si tout s'est bien passé ou -1 si une erreur a eu
 * lieu. */
int mutex_init(mutex_s * mutex);

/* Vérouille le mutex. Renvoie 0 si tout s'est bien passé ou -1 si une erreur a
 * eu lieu. */
int mutex_acquire(mutex_s * mutex);

/* Dévérouille le mutex. Renvoie 0 si tout s'est bien passé ou -1 si une erreur
 * a eu lieu. */
int mutex_release(mutex_s * mutex);

#endif
