#ifndef MUTEX_H
#define MUTEX_H

#include "types.h"

extern struct mutex_s;

/* Crée un mutex et renvoie son descripteur en modifiant *desc*. Renvoie 0 si
 * tout s'est bien passé ou -1 si une erreur a eu lieu. */
int mutex_create(mutex_s * desc);

/* Libère le mutex défini par *desc*. Renvoie 0 si tout s'est bien passé ou -1
 * si une erreur a eu lieu. */
int mutex_free(mutex_s * desc);

/* Vérouille le mutex défini par *desc*. Renvoie 0 si tout s'est bien passé ou
 * -1 si une erreur a eu lieu. */
int mutex_acquire(mutex_s * desc);

/* Dévérouille le mutex défini par *desc*. Renvoie 0 si tout s'est bien passé
 * ou -1 si une erreur a eu lieu. */
int mutex_release(mutex_s * desc);

#endif
