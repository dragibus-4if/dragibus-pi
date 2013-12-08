#include "sched.h"
#include "malloc.h"
#include "hw.h"

struct _prio_array {
    int nb_tasks;
    struct task_struct * list[MAX_PRIO << 1];
};

static time_t _uptime = 1;
static struct task_struct _idle_task;
static struct task_struct * _ready_list = NULL;
static struct task_struct * _waiting_list = NULL;
static struct task_struct * _current_task = NULL;
static struct _prio_array _prio_array[4];
static struct _prio_array * _active = &_prio_array[0];
static struct _prio_array * _expired = &_prio_array[1];
static struct _prio_array * _rt_active = &_prio_array[2];
static struct _prio_array * _rt_expired = &_prio_array[3];

void _add_last(struct task_struct ** list_head, struct task_struct * last) {
    /* Cas d'une liste vide */
    if (*list_head == NULL) {
        *list_head = last;
        last->next = last;
        last->prev = last;
    } else {
        last->prev = (*list_head)->prev;
        last->next = (*list_head);
        (*list_head)->prev->next = last;
        (*list_head)->prev = last;
    }
}

void _del_from(struct task_struct ** list_head, struct task_struct * task) {
    /* Cas d'un singleton */
    if (task == task->next) {
        *list_head = NULL;
    } else {
        task->prev->next = task->next;
        task->next->prev = task->prev;
    }
    task->prev = NULL;
    task->next = NULL;
}

/* Déplace la tache à la fin de la liste */
void _move_last(struct task_struct ** list_head, struct task_struct * last) {
    /* Cas particulier ou il faut déplacer la tete */
    if (last == *list_head) {
        *list_head = last->next;
        return;
    }

    /* Suppression de l'élément de la liste */
    last->prev->next = last->next;
    last->next->prev = last->prev;

    /* Ajout à la fin de la liste */
    last->prev = (*list_head)->prev;
    last->next = (*list_head);
    (*list_head)->prev->next = last;
    (*list_head)->prev = last;
}

/* Change le contexte d'exécution de *prev* à celui de *next* */
void _switch_to(struct task_struct * prev, struct task_struct * next) {
    __asm volatile("push {r0-r12, lr}");
    DISABLE_IRQ();
    __asm("mov %0, sp" : "=r"(prev->stack_pointer));
    __asm("mov sp, %0" : : "r"(next->stack_pointer));
    set_tick_and_enable_timer();
    __asm volatile("pop {r0-r12, lr}");
}

void _start_current_process(void) {
    set_current_state(TASK_READY);
    _current_task->entry_point(_current_task->args);
    set_current_state(TASK_ZOMBIE);
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

/* Renvoie la queue de tache active dans lequel la tache *task* doit etre */
struct task_struct ** _active_queue(struct task_struct * task) {
    /* Tache en temps réels */
    if (task->policy == SCHED_RT)
        return &_rt_active->list[task->rt_priority];

    /* Tache qui a demandé un yield */
    if (task->policy & SCHED_YIELD)
      return &_active->list[0];

    return &_active->list[task->priority + task->counter];
}

/* Renvoie la queue de tache expirée dans lequel la tache *task* doit etre */
struct task_struct ** _expired_queue(struct task_struct * task) {
    /* Tache en temps réels */
    if (task->policy == SCHED_RT)
        return &_rt_expired->list[task->rt_priority];

    /* Tache qui a demandé un yield */
    if (task->policy & SCHED_YIELD)
      return &_expired->list[0];

    return &_expired->list[task->priority + task->counter];
}

void _schedule(void) {
    /* Déplacement de la tache dans la case de prio correspondant */
    struct task_struct * prev = _current_task;
    if (prev != NULL && prev->state == TASK_READY) {
        if (prev->counter == 0) {
            /* Tache qui est épuisée */
            if (prev->policy == SCHED_RT) {
                /* Recalcule le temps de quantum de la tache et l'ajoute dans
                 * le tableau des taches expirées */
                prev->counter = prev->rt_priority;
                _rt_active->nb_tasks--;
                _rt_expired->nb_tasks++;
                if (prev->head != NULL) _del_from(&prev->head, prev);
                _add_last(_expired_queue(prev), prev);

                /* Si toute les taches sont épuisées on switch les deux
                 * tableaux */
                if (_rt_active->nb_tasks == 0) {
                    struct _prio_array * tmp = _rt_active;
                    _rt_active = _rt_expired;
                    _rt_expired = tmp;
                }
            } else {
                /* Recalcule le temps de quantum de la tache et l'ajoute dans
                 * le tableau des taches expirées */
                prev->counter = prev->priority;
                _active->nb_tasks--;
                _expired->nb_tasks++;
                if (prev->head != NULL) _del_from(&prev->head, prev);
                _add_last(_expired_queue(prev), prev);

                /* Si toute les taches sont épuisées on switch les deux
                 * tableaux */
                if (_active->nb_tasks == 0) {
                    struct _prio_array * tmp = _active;
                    _active = _expired;
                    _expired = tmp;
                }
            }
        } else if (prev->head != *_active_queue(prev)) {
            /* Si la priorité à été déplacé */
            prev->policy &= ~SCHED_YIELD;
            prev->need_resched = 0;
            if (prev->head != NULL) _del_from(&prev->head, prev);
            _add_last(_active_queue(prev), prev);
        }
    }

    /* Recherche du meilleur candidat parmi les taches actives */
    struct task_struct * next = NULL;
    for (ssize_t i = (MAX_PRIO << 1) - 1 ; i >= 0  ; i--) {
        if (_rt_active->list[i] != NULL) {
            next = _rt_active->list[i];
            break;
        } else if (next != NULL && _active->list[i] != NULL) {
            next = _active->list[i];
        }
    }

    /* Choix de idle si on trouve rien */
    if (next == NULL)
        next = &_idle_task;

    _switch_to(prev, next);
}

/* Called when there is a time interruption */
void __attribute__ ((naked)) __time_interrupt(void) {
    __asm volatile("sub lr, lr, #4");
    __asm volatile("srsdb sp!, 0x13");
    __asm volatile("cps #0x13");

    /* Met à jour l'epoch système */
    _uptime++;

    /* Enlève un tick au quantum de la tache courante */
    _current_task->counter--;

    /* Vérifie si il faut faire un changement de tache */
    if (_current_task->need_resched || _current_task->counter == 0) {
        _schedule();
    }

    __asm volatile ("rfefd sp!");
}

void sched_start(void) {
    _idle_task.prev = NULL;
    _idle_task.next = NULL;
    _current_task = &_idle_task;
    for(int i = 0 ; i < 4 ; i++) {
        _prio_array[i].nb_tasks = 0;
        for(int j = 0 ; j < MAX_PRIO << 1 ; j++)
            _prio_array[i].list[j] = NULL;
    }
    sched_yield();
    while (1);
}

void sched_yield(void) {
    _current_task->need_resched = 1;
    _current_task->policy |= SCHED_YIELD;
}

void sched_forced_yield(void) {
    _current_task->need_resched = 1;
    _current_task->policy |= SCHED_YIELD;
    _schedule();
}

int create_process(func_t * f, void * args, size_t size) {
    struct task_struct * task;
    task = (struct task_struct *) malloc_alloc(sizeof(struct task_struct));
    if (task == NULL) {
        return -1;
    }

    task->next = NULL;
    task->prev = NULL;
    task->head = NULL;

    return _init_process(task, size, f, args);
}

struct task_struct * get_current_process(void) {
    return _current_task;
}

int set_current_state(enum task_state state) {
    return set_process_state(_current_task, state);
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
        int y = task == _current_task;
        malloc_free((char *) task->stack_base);
        malloc_free((char *) task);
        if(y) {
            _current_task = NULL;
            _schedule();
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
    if(task->state == TASK_WAITING && task == _current_task) {
        _schedule();
    }

    return 0;
}
