#ifndef SCHED_H
#define SCHED_H

#include "types.h"

enum task_state { TASK_NEW, TASK_READY, TASK_WAITING, TASK_ZOMBIE };
enum sched_policy { SCHED_FIFO, SCHED_RR, SCHED_OTHER, SCHED_YIELD };

struct task_struct {
    /* Fonction et arguments */
    func_t * entry_point;
    void * args;

    /* State */
    enum task_state state;

    /* Schedule policy */
    int policy;

    /* Time informations */
    time_t epoch;
    time_t counter;

    /* Priority */
    int priority;
    int rt_priority;

    /* Pointeur de pile */
    uint32_t * stack_pointer;
    char * stack_base;
    size_t stack_size;

    /* Double linked list */
    struct task_struct * next;
    struct task_struct * prev;
};

/* Gestion de l'ordonnancement */
void ctx_switch(void);
void yield(void);
void schedule(void);
void start_sched(void);

/* Gestion des taches */
int create_process(func_t * f, void * args, size_t size);
int set_process_state(struct task_struct * task, enum task_state state);
int set_current_state(enum task_state state);
struct task_struct * get_current_process(void);

#endif
