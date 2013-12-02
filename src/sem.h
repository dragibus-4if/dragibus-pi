#ifndef SEM_H
#define SEM_H

struct pcb_Sem {
    struct pcb_s* pcb;
    struct pcb_Sem* next;
}

struct sem_s {
    int32_t counter;
    struct pcb_Sem* pcbSemF;
    struct pcb_Sem* pcbSemL;
}

void sem_init(struct sem_s* sem, uint32_t val);

#endif
