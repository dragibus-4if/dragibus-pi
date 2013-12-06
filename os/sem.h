#ifndef SEM_H
#define SEM_H

#include "types.h"

/* Structure de données représentant une sémapore */
struct pcb_list;
struct sem_s {
    int counter;
    struct pcb_list * waiting_queue;
};

struct pcb_list {
    struct pcb_s * pcb;
    struct pcb_list * next;
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
