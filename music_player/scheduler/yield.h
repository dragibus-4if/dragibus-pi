#ifndef SCHED_H
#define SCHED_H

#include <stddef.h>
#include <stdint.h>
#include "types.h"

/* Gestion de la priorité */
#define MAX_PRIORITY 10
typedef unsigned short int priority_t;

enum sched_mode_e { BASIC, PRIORITY };

enum task_state { TASK_NEW, TASK_READY, TASK_WAITING, TASK_ZOMBIE };

struct task_struct {
    /* Fonction et arguments */
    func_t * entry_point;
    void * args;

    /* State and priority */
    enum task_state state;
    priority_t priority;

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
void set_sched_mode(enum sched_mode_e mode);

/* Gestion des taches */
int create_process(func_t * f, void * args, size_t size);
int set_process_state(struct task_struct * task, enum task_state state);
int set_current_state(enum task_state state);
struct task_struct * get_current_process(void);

#endif
