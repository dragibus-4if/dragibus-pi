#include "sem.h"
#include "sched.h"
#include "malloc.h"
#include "hw.h"

struct _pcb_sem_s {
    struct pcb_s * pcb;
    struct _pcb_sem_s * next;
};

struct sem_s {
    int counter;
    struct _pcb_sem_s * first_pcbs;
    struct _pcb_sem_s * last_pcbs;
};

void sem_init(struct sem_s * sem, int val) {
    /*sem = (struct sem_s *)malloc_alloc(sizeof(struct sem_s));*/
    sem->counter = 0;
    sem->counter += val;
    sem->first_pcbs = NULL;
    sem->last_pcbs = NULL;
}

void sem_up(struct sem_s * sem) {
    DISABLE_IRQ();
    sem->counter++;
    if (sem->counter <=0){
        process_release(sem->first_pcbs->pcb);
        /*struct _pcb_sem_s* temp = sem->first_pcbs;*/
        sem->first_pcbs=sem->first_pcbs->next;
    }
    ENABLE_IRQ();
    /*malloc_free(temp);*/
}

void sem_down(struct sem_s * sem){
    DISABLE_IRQ();
    sem->counter--;
    if (sem->counter < 0) {
        struct _pcb_sem_s * npcbs = (struct _pcb_sem_s*) malloc_alloc(sizeof(struct _pcb_sem_s));
        npcbs->pcb = get_current_process();
        if (sem->first_pcbs == NULL ){
            sem->first_pcbs = npcbs;
            sem->last_pcbs = npcbs;
        } else {
            sem->last_pcbs->next = npcbs;
            sem->last_pcbs = npcbs;
        }
        process_block();
    }
    ENABLE_IRQ();
}
