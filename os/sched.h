#ifndef SCHED_H
#define SCHED_H

#include "types.h"

#define MAX_PRIO 10

enum task_state { TASK_NEW, TASK_READY, TASK_WAITING, TASK_ZOMBIE };
enum sched_policy { SCHED_RT = 1, SCHED_OTHER = 2, SCHED_YIELD = 4 };

struct task_struct {
    /* Fonction et arguments */
    func_t * entry_point;
    void * args;

    /* State */
    enum task_state state;

    /* Schedule policy */
    int policy;
    int need_resched;

    /* Time informations */
    time_t epoch;
    time_t counter;

    /* Priority */
    time_t priority;
    time_t rt_priority;
    time_t current_prio;

    /* Pointeur de pile */
    uint32_t * stack_pointer;
    char * stack_base;
    size_t stack_size;

    /* Double linked list */
    struct task_struct * next;
    struct task_struct * prev;
};

/* Commence à exécuter l'ordonnancement */
void sched_start(void);

/* La tache courante se place à la fin de la file d'exécution et une nouvelle
 * tache est activé au prochain epoch */
void sched_yield(void);

/* Force le scheduler à changer directement de tache */
void sched_forced_yield(void);

/* Gestion des taches */
int create_process(func_t * f, void * args, size_t size, int
    policy, int prio);
int set_process_state(struct task_struct * task, enum task_state state);
int set_current_state(enum task_state state);
struct task_struct * get_current_process(void);

#endif
