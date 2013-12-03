#ifndef MUTEX_H
#define MUTEX_H

/* Crée un mutex et renvoie son descripteur en modifiant *desc*. Renvoie 0 si
 * tout s'est bien passé ou -1 si une erreur a eu lieu. */
int mutex_create(intptr_t * desc);

/* Libère le mutex défini par *desc*. Renvoie 0 si tout s'est bien passé ou -1
 * si une erreur a eu lieu. */
int mutex_free(intptr_t desc);

/* Vérouille le mutex défini par *desc*. Renvoie 0 si tout s'est bien passé ou
 * -1 si une erreur a eu lieu. */
void mutex_acquire(intptr_t desc);

/* Dévérouille le mutex défini par *desc*. Renvoie 0 si tout s'est bien passé
 * ou -1 si une erreur a eu lieu. */
void mutex_release(intptr_t desc);

#endif
