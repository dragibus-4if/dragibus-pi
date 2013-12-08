#include "sched.h"
#include "malloc.h"
#include "hw.h"

static time_t _uptime = 1;
static struct task_struct _idle_task;
static struct task_struct * _ready_list = NULL;
static struct task_struct * _waiting_list = NULL;
static struct task_struct * _current_process = NULL;

/* Change le contexte d'exécution de *prev* à celui de *next* */
void _switch_to(struct task_struct * prev, struct task_struct * next) {
    __asm volatile("push {r0-r12, lr}");
    DISABLE_IRQ();
    __asm("mov %0, sp" : "=r"(prev->stack_pointer));
    __asm("mov sp, %0" : : "r"(next->stack_pointer));
    set_tick_and_enable_timer();
    __asm volatile("pop {r0-r12, lr}");
}

/* Mesure le poids en priorité d'une tache *p* */
int _goodness(struct task_struct * p) {
    /* On ne doit pas choisir le IDLE */
    if (p == &_idle_task)
        return -1000;

    /* Si c'est une tache en real time */
    if (p->policy != SCHED_OTHER)
        return 1000 + p->rt_priority;

    /* Quand le quantum de la tache est fini */
    if (p->counter == 0)
        return 0;

    /* Cas normal */
    return p->counter + p->priority;
}

int _init_process(struct task_struct *task,
        size_t stack_size,
        func_t * f,
        void * args) {
    /* Function and args */
    task->entry_point = f;
    task->args = args;

    /* Stack allocation */
    task->stack_size = stack_size;
    task->stack_base = malloc_alloc(stack_size);
    if (task->stack_base == NULL) {
        return -1;
    }

    /* Policy */
    task->policy = SCHED_OTHER;
    task->need_resched = 0;

    /* Priority and time */
    task->priority = 0;
    task->rt_priority = 0;
    task->counter = 0;
    task->epoch = 0;

    /* State and context */
    set_process_state(task, TASK_NEW);
    task->stack_pointer = ((uint32_t *) (task->stack_base + stack_size)) - 1;

    /* Fill in the stack with CPSR and PC */
    *(task->stack_pointer) = 0x53;
    task->stack_pointer--;
    *(task->stack_pointer) = (unsigned int) &_start_current_process;

    return 0;
}

void _start_current_process(void) {
    set_current_state(TASK_READY);
    _current_process->entry_point(_current_process->args);
    set_current_state(TASK_ZOMBIE);
}

void _schedule(void) {
    struct task_struct * prev = _current_process;
    struct task_struct * next = NULL;
    int c = 0;

    /* Tache en RoundRobin temps réel qui est épuisée */
    if (prev->counter == 0 && prev->policy == SCHED_RR) {
        prev->counter = prev->priority;
        _move_last(&_ready_list, prev);
    } else if (prev->state == TASK_RUNNING) {
        next = prev;

        /* Si la tache à demandé à changer, on lui donne une priorité très
         * faible */
        if (prev->policy & SCHED_YIELD) {
            prev->policy &= ~SCHED_YIELD;
            c = 0;
        } else
          c = _goodness(prev);
    } else {
        next = &_idle_task;
        c = -1000;
    }

    /* TODO parcourt sur les runnable tasks et choix du meilleur */

    /* Si on a des process prets */
    if (_ready_list != NULL) {
        /* Rajout de ce task à la fin de la liste */
        _current_process = _ready_list;
        _ready_list = _ready_list->next;
    } else {
        /* Sinon on utilise le task idle */
        _current_process = &_idle_task;
    }
}

/* void yield(void) { */
/*     __asm volatile("sub lr, lr, #4"); */
/*     __asm volatile("srsdb sp!, 0x13"); */
/*     __asm volatile("cps #0x13"); */
/*  */
/*     __asm volatile("push {r0-r12, lr}"); */
/*     DISABLE_IRQ(); */
/*  */
/*     __asm("mov %0, sp" : "=r"(_current_process->stack_pointer)); */
/*     schedule(); */
/*     __asm("mov sp, %0" : : "r"(_current_process->stack_pointer)); */
/*  */
/*     set_tick_and_enable_timer(); */
/*     if (_current_process->state == TASK_NEW) { */
/*         ENABLE_IRQ(); */
/*         _start_current_process(); */
/*     } else { */
/*         __asm volatile("pop {r0-r12, lr}"); */
/*     } */
/*     __asm volatile ("rfefd sp!"); */
/* } */

/* Called when there is a time interruption */
void __attribute__ ((naked)) __time_interruption(void) {
    __asm volatile("sub lr, lr, #4");
    __asm volatile("srsdb sp!, 0x13");
    __asm volatile("cps #0x13");

    /* Met à jour l'epoch système */
    _uptime++;

    /* Enlève un tick au quantum de la tache courante */
    _current_process->counter--;

    /* Vérifie si il faut faire un changement de tache */
    if (_current_process->need_resched || _current_process->counter == 0) {
        _schedule();
    }

    __asm volatile ("rfefd sp!");
}

void sched_start(void) {
    _idle_task.prev = NULL;
    _idle_task.next = NULL;
    _current_process = &_idle_task;
    sched_yield();
    while (1);
}

void sched_yield(void) {
    _current_process->need_resched = 1;
    _current_process->policy |= SCHED_YIELD;
    _move_last(&_ready_list, _current_process);
}

int create_process(func_t * f, void * args, size_t size) {
    struct task_struct * task;
    task = (struct task_struct *) malloc_alloc(sizeof(struct task_struct));
    if (task == NULL) {
        return -1;
    }

    task->next = NULL;
    task->prev = NULL;

    return _init_process(task, size, f, args);
}

struct task_struct * get_current_process(void) {
    return _current_process;
}

int set_current_state(enum task_state state) {
    return set_process_state(_current_process, state);
}

int set_process_state(struct task_struct * task, enum task_state state) {
    /* Vérif des valeurs */
    if (task == NULL) {
        return -1;
    }
    if (state != TASK_NEW && task->state == state) {
        return 0;
    }

    /* Gestion du cas particulier ou on passe de TASK_NEW à TASK_READY */
    if (task->state == TASK_NEW && state == TASK_READY) {
        task->state = TASK_READY;
        return 0;
    }

    /* On retire le task de sa liste de state courant (s'il en a une) */
    if(task->next != NULL && task->prev != NULL) {
        if (task->next == task) {
            /* Dans le cas de un seul élément, la liste devient vide */
            if (task->state == TASK_READY) {
                _ready_list = NULL;
            } else if (task->state == TASK_WAITING) {
                _waiting_list = NULL;
            }
        } else {
            task->prev->next = task->next;
            task->next->prev = task->prev;
            if (task == _ready_list) {
                _ready_list = task->next;
            } else if (task == _waiting_list) {
                _waiting_list = task->next;
            }
        }
    }

    /* Dans le cas ou le processus est fini, on le tue */
    if (state == TASK_ZOMBIE) {
        int y = task == _current_process;
        malloc_free((char *) task->stack_base);
        malloc_free((char *) task);
        if(y) {
            yield();
        }
        return 0;
    }

    /* On l'ajoute à la fin de sa nouvelle liste */
    /* TODO si on utilise la priorité on peut faire une
     * insertion trié ici-meme */
    if (state == TASK_READY || state == TASK_NEW) {
        if (_ready_list == NULL) {
            _ready_list = task;
            task->prev = task;
            task->next = task;
        } else {
            task->prev = _ready_list->prev;
            task->next = _ready_list;
            _ready_list->prev->next = task;
            _ready_list->prev = task;
        }
    } else if (state == TASK_WAITING) {
        if (_waiting_list == NULL) {
            _waiting_list = task;
            task->prev = task;
            task->next = task;
        } else {
            task->prev = _waiting_list->prev;
            task->next = _waiting_list;
            _waiting_list->prev->next = task;
            _waiting_list->prev = task;
        }
    }

    task->state = state;

    /* Dans le cas ou la tache courante est en attente, on change */
    if(task->state == TASK_WAITING && task == _current_process) {
        yield();
    }

    return 0;
}
