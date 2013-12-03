#include "sem.h"
#include "sched.h"

void
sem_init(struct sem_s *sem, int32_t val){
    sem = malloc_init(sem);
    sem->counter = val;
    sem->pcbSemF = NULL;
    sem->pcbSemL = NULL; 
}

void
sem_up(struct sem_s* sem){
    sem->counter++;
    // TODO ecrire process_release
    process_release(sem->pcbSemF->pcb);
    struct sem_s* temp = sem->pcbSemF;
    sem->pcbSemF=sem->pcbSemF->next;  
    malloc_free(temp);  
}

void sem_down(struct sem_s* sem){
    sem->counter--;
    if(sem->counter<=0){
        struct pcb_Sem* nPcbSem = malloc_init(nPcbSem);
        nPcbSem->pcb = current_process;
        if(sem->pcbSemF==NULL){
            sem->pcbSemF=nPcbSem;
            sem->pcbSemL=nPcbSem;
        }
        else{       
            sem->pcbSemL->next = nPcbSem;
            sem->pcbSemL = nPcbSem;       
        }   
        //TODO ecrire process_blocks
        process_block();
    }
}