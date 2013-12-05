#include "sem.h"
#include "sched.h"
#include "malloc.h"
void
sem_init(struct sem_s *sem, int val){
    sem = (struct sem_s*) malloc_alloc(1000/*sizeof(struct sem_s)*/);
    sem->counter = 0;
    sem->counter += val;
    sem->pcbSemF = NULL;
    sem->pcbSemL = NULL; 
}

void
sem_up(struct sem_s* sem){
    sem->counter++;
    process_release(sem->pcbSemF->pcb);
    struct pcb_Sem* temp = sem->pcbSemF;
    sem->pcbSemF=sem->pcbSemF->next;  
    malloc_free(temp);  
}

void sem_down(struct sem_s* sem){
    sem->counter--;
    if(sem->counter<0){
        struct pcb_Sem* nPcbSem = (struct pcb_Sem*) malloc_alloc(sizeof(struct pcb_Sem));
        nPcbSem->pcb = current_process;
        if(sem->pcbSemF==NULL){
            sem->pcbSemF=nPcbSem;
            sem->pcbSemL=nPcbSem;
        }
        else{       
            sem->pcbSemL->next = nPcbSem;
            sem->pcbSemL = nPcbSem;       
        }   
        process_block();
    }
}
