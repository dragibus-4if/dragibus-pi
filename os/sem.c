#include "sem.h"
#include "sched.h"
#include "malloc.h"
#include "hw.h"

void sem_init(struct sem_s * sem, int val) {
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
        /*struct sem_pcb_s* temp = sem->first_pcbs;*/
        sem->first_pcbs=sem->first_pcbs->next;
    }
    ENABLE_IRQ();
    /*malloc_free(temp);*/
}

void sem_down(struct sem_s * sem){
    DISABLE_IRQ();
    sem->counter--;
    if (sem->counter < 0) {
        struct sem_pcb_s * npcbs = (struct sem_pcb_s*) malloc_alloc(sizeof(struct sem_pcb_s));
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
