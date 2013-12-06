#ifndef SEM_H
#define SEM_H

#include "types.h"

/* Structure de données représentant une sémapore */
struct sem_pcb_s;
struct sem_s {
    int counter;
    struct sem_pcb_s * first_pcbs;
    struct sem_pcb_s * last_pcbs;
};

struct sem_pcb_s {
    struct pcb_s * pcb;
    struct sem_pcb_s * next;
};

/* Initialize une sémaphore déjà allouée */
void sem_init(struct sem_s * sem, int val);

/* Incrémente la valeur d'une sémaphore, plaçant le processus en attente quand
 * la valeur max a été atteinte */
void sem_up(struct sem_s * sem);

/* Décrémente la valeur d'une sémaphore, plaçant le processus en attente quand
 * la valeur min a été atteinte */
void sem_down(struct sem_s * sem);

#endif
