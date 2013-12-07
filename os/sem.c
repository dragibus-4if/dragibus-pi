#include "sem.h"
#include "sched.h"
#include "malloc.h"
#include "hw.h"

struct task_queue {
    struct task_struct * task;
    struct task_queue * next;
};

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
        set_process_state(sem->waiting_queue->task, TASK_READY);
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
        /* Création d'un nouvel élément de liste */
        struct task_queue * p = (struct task_queue *) malloc_alloc(
            sizeof(struct task_queue));
        p->next = NULL;
        p->task = get_current_process();

        /* Ajout à la fin de la liste d'attente */
        if (sem->waiting_queue == NULL) {
            sem->waiting_queue = p;
        } else {
            struct task_queue * last;
            for (last = sem->waiting_queue;
                last->next != NULL;
                last = last->next);
            last->next = p;
        }

        /* Bloque la tache courante */
        set_current_state(TASK_WAITING);
    }

    ENABLE_IRQ();
    return 0;
}
