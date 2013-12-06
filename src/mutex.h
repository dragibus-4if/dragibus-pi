#ifndef MUTEX_H
#define MUTEX_H

#include "types.h"
#include "sem.h"
#include "sched.h"

/* Structure de données représentant un mutex */
struct mutex_s {
    struct pcb_s * owner;
    struct sem_s sem;
};

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
