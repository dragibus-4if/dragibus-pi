#include "sem.h"
#include "sched.h"
#include "malloc.h"
#include "hw.h"

int sem_init(struct sem_s * sem, int val) {
    /* Vérification des entrées */
    if(sem == NULL) {
        return -1;
    }

    if(val <= 0) {
        return -1;
    }

    sem->counter = val;
    sem->waiting_queue = NULL;
    return 0;
}

int sem_up(struct sem_s * sem) {
    DISABLE_IRQ();

    /* Vérification de l'entrée */
    if (sem == NULL) {
        ENABLE_IRQ();
        return -1;
    }

    sem->counter++;
    if (sem->counter <= 0) {
        /* Active une tache dans la file d'attente */
        set_process_state(sem->waiting_queue->pcb, READY);
        sem->waiting_queue = sem->waiting_queue->next;
    }

    ENABLE_IRQ();
    return 0;
}

int sem_down(struct sem_s * sem){
    DISABLE_IRQ();

    /* Vérification de l'entrée */
    if (sem == NULL) {
        ENABLE_IRQ();
        return -1;
    }

    sem->counter--;
    if (sem->counter < 0) {
        /* Bloque la tache courante */
        set_current_state(WAITING);

        /* Création d'un nouvel élément de liste */
        struct pcb_list * p = (struct pcb_list *) malloc_alloc(
            sizeof(struct pcb_list));
        p->next = NULL;
        p->pcb = get_current_process();

        /* Ajout à la fin de la liste d'attente */
        if (sem->waiting_queue == NULL) {
            sem->waiting_queue = p;
        } else {
            struct pcb_list * last;
            for (last = sem->waiting_queue;
                last->next != NULL;
                last = last->next);
            last->next = p;
        }
    }

    ENABLE_IRQ();
    return 0;
}
