#ifndef SEM_H
#define SEM_H

#include "types.h"

struct task_queue;

/* Structure de données représentant une sémapore */
struct sem_s {
    int counter;
    struct task_queue * waiting_queue;
};

/* Initialize une sémaphore déjà allouée */
int sem_init(struct sem_s * sem, int val);

/* Incrémente la valeur d'une sémaphore, plaçant le processus en attente quand
 * la valeur max a été atteinte */
int sem_up(struct sem_s * sem);

/* Décrémente la valeur d'une sémaphore, plaçant le processus en attente quand
 * la valeur min a été atteinte */
int sem_down(struct sem_s * sem);

#endif
