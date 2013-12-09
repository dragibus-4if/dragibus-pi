#include "sched.h"
#include "malloc.h"
#include "hw.h"

struct _prio_array {
    int nb_tasks;
    struct task_struct * list[MAX_PRIO << 1];
};

struct task_struct * _add_last(struct task_struct * list_head, struct task_struct * last);
struct task_struct * _del_from(struct task_struct * list_head, struct task_struct * task);

/* Change le contexte d'exécution de *_current_task* à celui de *_next_task* */
void _switch_to(void);

int _init_process(struct task_struct *task,
        size_t stack_size,
        func_t * f,
        void * args);
void _schedule(void);

/* Retourne le counter d'exécution de base d'une tache */
time_t _get_base_counter(struct task_struct * task);

static time_t _uptime = 1;
static struct task_struct _idle_task;
static struct task_struct * _waiting_list = NULL;
static struct task_struct * _current_task = NULL;
static struct task_struct * _next_task = NULL;
static struct _prio_array _prio_array[4];
static struct _prio_array * _active = &_prio_array[0];
static struct _prio_array * _expired = &_prio_array[1];
static struct _prio_array * _rt_active = &_prio_array[2];
static struct _prio_array * _rt_expired = &_prio_array[3];

struct task_struct * _add_last(struct task_struct * list_head,
    struct task_struct * last) {
    /* Cas d'une liste vide */
    if (list_head == NULL) {
        list_head = last;
        last->next = last;
        last->prev = last;
    } else {
        last->prev = list_head->prev;
        last->next = list_head;
        list_head->prev->next = last;
        list_head->prev = last;
    }
    return list_head;
}

struct task_struct * _del_from(struct task_struct * list_head,
    struct task_struct * task) {
    /* Cas d'un singleton */
    if (task == task->next) {
        list_head = NULL;
    } else {
        task->prev->next = task->next;
        task->next->prev = task->prev;
        if(task == list_head)
            list_head = task->next;
    }
    task->prev = NULL;
    task->next = NULL;
    return list_head;
}

/* Change le contexte d'exécution de *prev* à celui de *next* */
void _switch_to(void) {
    __asm volatile("push {r0-r12, lr}");
    DISABLE_IRQ();
    __asm("mov %0, sp" : "=r"(_current_task->stack_pointer));
    _current_task = _next_task;
    __asm("mov sp, %0" : : "r"(_current_task->stack_pointer));
    ENABLE_IRQ();
    set_tick_and_enable_timer();
    if (_current_task->state == TASK_NEW) {
        _current_task->state = TASK_READY;
        _current_task->entry_point(_current_task->args);
        set_current_state(TASK_ZOMBIE);
    } else {
        __asm volatile("pop {r0-r12, lr}");
    }
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
    task->priority = 20;
    task->rt_priority = 0;
    task->counter = _get_base_counter(task);
    task->current_prio = task->counter;
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

time_t _get_base_counter(struct task_struct * task) {
    if (task->policy == SCHED_RT)
        return task->rt_priority;
    return task->priority << 1;
}

/* Change le counter de la tache et la place dans le bonne file d'attente */
void _set_counter(struct task_struct * task, time_t counter) {
    if (task->counter == counter && task->current_prio == counter) {
        return;
    }

    /* Quand la tache n'est pas en cours d'exécution, seule son counter change
     * mais elle reste dans la file d'attente */
    if (task->state != TASK_READY) {
        if (counter == 0)
            task->counter = _get_base_counter(task);
        else
            task->counter = counter;
        return;
    }

    struct _prio_array * active = _active;
    struct _prio_array * expired = _expired;
    if (task->policy == SCHED_RT) {
        active = _rt_active;
        expired = _rt_expired;
    }

    /* Suppression dans l'ancienne queue d'exécution */
    active->list[task->current_prio] =
      _del_from(active->list[task->current_prio], task);
    active->nb_tasks--;

    /* Avec un counter nul, on place la tache dans les expirées et on lui donne
     * un nouveau counter */
    if (counter == 0) {
        time_t new_counter = _get_base_counter(task);
        /* Ajout à la fin de la nouvelle */
        expired->list[new_counter] =
            _add_last(expired->list[new_counter], task);
        expired->nb_tasks++;
        task->counter = new_counter;
        task->current_prio = new_counter;
        return;
    }

    /* Sinon, on place la tache dans le nouveau tableau */
    active->list[counter] =
      _add_last(active->list[counter], task);
    active->nb_tasks++;
    task->counter = counter;
    task->current_prio = counter;
}

void _schedule(void) {
    DISABLE_IRQ();
    struct task_struct * prev = _current_task;

    /* Si la tache à demandé un yield on la met en moins prio */
    if (prev != &_idle_task
        && prev->state == TASK_READY
        && prev->policy & SCHED_YIELD) {
        _active->list[prev->current_prio] =
          _del_from(_active->list[prev->current_prio], prev);
        _active->list[0] =
          _add_last(_active->list[0], prev);
        prev->policy &= ~SCHED_YIELD;
        prev->need_resched = 0;
        prev->current_prio = 0;
    }

    /* Si toute les taches sont épuisées on switch les deux tableaux */
    if (_rt_active->nb_tasks == 0) {
      struct _prio_array * tmp = _rt_active;
      _rt_active = _rt_expired;
      _rt_expired = tmp;
    }
    if (_active->nb_tasks == 0) {
      struct _prio_array * tmp = _active;
      _active = _expired;
      _expired = tmp;
    }

    /* Recherche du meilleur candidat parmi les taches actives */
    struct task_struct * next = NULL;
    for (ssize_t i = (MAX_PRIO << 1) - 1 ; i >= 0  ; i--) {
        if (_rt_active->list[i] != NULL) {
            next = _rt_active->list[i];
            break;
        } else if (next == NULL && _active->list[i] != NULL) {
            next = _active->list[i];
        }
    }

    /* Choix de idle si on trouve rien */
    if (next == NULL)
        next = &_idle_task;

    _next_task = next;

    if (next != prev)
      _switch_to();
}

/* Called when there is a time interruption */
void __attribute__ ((naked)) __time_interrupt(void) {
    __asm volatile("sub lr, lr, #4");
    __asm volatile("srsdb sp!, 0x13");
    __asm volatile("cps #0x13");

    /* Met à jour l'epoch système */
    _uptime++;

    /* Enlève un tick au quantum de la tache courante */
    _set_counter(_current_task, _current_task->counter - 1);

    /* Vérifie si il faut faire un changement de tache */
    if (_current_task->need_resched || _current_task->counter == 0) {
        _schedule();
    }

    __asm volatile ("rfefd sp!");
}

void sched_start(void) {
    DISABLE_IRQ();
    _idle_task.prev = NULL;
    _idle_task.next = NULL;
    _current_task = &_idle_task;
    /* for(int i = 0 ; i < 4 ; i++) */
    /*     _prio_array[i].nb_tasks = 0; */
    _schedule();
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
    if (task == &_idle_task) {
        return 0;
    }

    struct _prio_array * active = _active;
    if (task->policy == SCHED_RT)
        active = _rt_active;

    /* Si la tache n'est plus en exécution on l'enlève des files */
    if (task->state == TASK_READY) {
        active->nb_tasks--;
        active->list[task->current_prio] =
          _del_from(active->list[task->current_prio], task);
    }

    /* Si la tache était en attente, on l'enlève de la waiting queue */
    if (task->state == TASK_WAITING) {
        _waiting_list = _del_from(_waiting_list, task);
    }

    /* Si on passe en exécution */
    if (state == TASK_READY || state == TASK_NEW) {
        active->nb_tasks++;
        active->list[task->counter] =
          _add_last(active->list[task->counter], task);
    }

    /* Si on passe en attente on l'ajoute dans la waiting list */
    if (state == TASK_WAITING) {
        _waiting_list =
          _add_last(_waiting_list, task);
    }

    /* Si la tache est fini, on la supprime */
    if (state == TASK_ZOMBIE) {
        malloc_free((char *) task->stack_base);
        malloc_free((char *) task);
        _schedule();
        return 0;
    }

    task->state = state;
    if (task->state == TASK_WAITING)
        _schedule();

    return 0;
}
