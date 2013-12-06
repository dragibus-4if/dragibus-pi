#ifndef SCHED_H
#define SCHED_H

#include "types.h"

typedef void (func_t)(void *);

enum pcb_state_e { READY, NEW, TERMINATED, WAITING };

/* Gestion de la priorité */
#define MAX_PRIORITY 10
typedef unsigned short int priority_t;

enum sched_mode_e { BASIC, PRIORITY };

struct pcb_s {
    /* Pointeur de pile */
    uint32_t * sp;

    /* Fonction et arguments */
    func_t * entry_point;
    void * args;

    size_t size;
    char * stack_base;
    enum pcb_state_e state;

    struct pcb_s * next;
    priority_t priority;
};

/* Gestion de l'ordonnancement */
static inline void yield() {
    ctx_switch();
}

void schedule();
void start_sched();
void set_sched_mode(enum sched_mode_e mode);

/* Gestion d'un processus */
int create_process(func_t * f, void * args, size_t size);
void process_block();
void process_release(struct pcb_s* pcb);
int set_process_state(struct pcb_s * pcb, enum pcb_state_e state);
int set_current_state(enum pcb_state_e state);

/* Gestion du process courant */
struct pcb_s * get_current_process();

#endif
