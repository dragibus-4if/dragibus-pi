#ifndef MUTEX_H
#define MUTEX_H

#include "types.h"

/* Structure de données (privée) du mutex */
struct mutex_s;

/* Crée dynamiquement un mutex. Renvoie 0 si tout s'est bien passé ou -1 si une
 * erreur a eu lieu. */
struct mutex_s * mutex_create();

/* Libère dynamiquement un mutex. Renvoie 0 si tout s'est bien passé ou -1 si une
 * erreur a eu lieu. */
int mutex_free(struct mutex_s * mutex);

/* Crée un mutex. Renvoie 0 si tout s'est bien passé ou -1 si une erreur a eu
 * lieu. */
int mutex_init(struct mutex_s * mutex);

/* Vérouille le mutex. Renvoie 0 si tout s'est bien passé ou -1 si une erreur a
 * eu lieu. */
int mutex_acquire(struct mutex_s * mutex);

/* Dévérouille le mutex. Renvoie 0 si tout s'est bien passé ou -1 si une erreur
 * a eu lieu. */
int mutex_release(struct mutex_s * mutex);

#endif
