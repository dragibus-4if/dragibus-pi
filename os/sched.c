#include "sched.h"
#include "malloc.h"
#include "hw.h"

static struct task_struct * _sort_task_list(struct task_struct * task) {
    /* Cas de base */
    if (task == NULL || task->next == NULL) {
        return task;
    }

    /* Choix du point de pivot */
    static const size_t pivot_choice = 0;
    struct task_struct * pivot = task;
    for (size_t i = 0; i < pivot_choice && pivot->next != NULL; i++) {
        pivot = pivot->next;
    }

    /* Enlever le pivot de la liste */
    struct task_struct * next, * tmp = NULL;
    while (task != NULL) {
        next = task->next;
        if (task->priority != pivot->priority) {
            task->next = tmp;
            tmp = task;
        }
        task = next;
    }

    /* Divide & conquer */
    struct task_struct * first = NULL, * second = NULL;
    while (tmp != NULL) {
        next = tmp->next;
        /* FIXME selon l'ordre du tri, changer "<" par ">" */
        if (tmp->priority < pivot->priority) {
            tmp->next = first;
            first = tmp;
        } else {
            tmp->next = second;
            second = tmp;
        }
        tmp = next;
    }

    /* Appels récursifs */
    first = _sort_task_list(first);
    second = _sort_task_list(second);

    /* Merge */
    if (first != NULL) {
        struct task_struct * end = first;
        while (end->next != NULL) {
            end = end->next;
        }
        pivot->next = second;
        end->next = pivot;
        return first;
    } else {
        pivot->next = second;
        return pivot;
    }
}

static struct task_struct * _schedule_priority(struct task_struct * task) {
    /* TODO */
    return task;
}

static struct task_struct _idle_process;
static struct task_struct * _ready_list = NULL;
static struct task_struct * _waiting_list = NULL;
static struct task_struct * _current_process = NULL;

/* Ordonnanceur à priority */
static enum sched_mode_e _sched_mode = BASIC;

struct task_struct * get_current_process(void) {
    return _current_process;
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

int set_current_state(enum task_state state) {
    return set_process_state(_current_process, state);
}

void _start_current_process(void) {
    set_current_state(TASK_READY);
    ENABLE_IRQ();
    _current_process->entry_point(_current_process->args);
    DISABLE_IRQ();
    set_current_state(TASK_ZOMBIE);
}

int _init_process(struct task_struct *task, size_t stack_size, func_t * f, void * args) {
    /* Function and args */
    task->entry_point = f;
    task->args = args;

    /* Stack allocation */
    task->stack_size = stack_size;
    task->stack_base = malloc_alloc(stack_size);
    if (task->stack_base == NULL) {
        return -1;
    }

    /* Priority */
    task->priority = 0;

    /* State and context */
    set_process_state(task, TASK_NEW);
    task->stack_pointer = ((uint32_t *) (task->stack_base + stack_size)) - 1;

    /* Fill in the stack with CPSR and PC */
    *(task->stack_pointer) = 0x53;
    task->stack_pointer--;
    *(task->stack_pointer) = (unsigned int) &_start_current_process;

    return 0;
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

void schedule(void) {
    /* Si on a des process prets */
    if (_ready_list != NULL) {
        /* Rajout de ce task à la fin de la liste */
        _current_process = _ready_list;
        _ready_list = _ready_list->next;
    } else {
        /* Sinon on utilise le task idle */
        _current_process = &_idle_process;
    }
}

void start_sched(void) {
    _idle_process.prev = NULL;
    _idle_process.next = NULL;
    _current_process = &_idle_process;
    yield();
    while (1);
}

void yield(void) {
    __asm volatile("push {r0-r12, lr}");
    DISABLE_IRQ();

    __asm("mov %0, sp" : "=r"(_current_process->stack_pointer));
    schedule();
    __asm("mov sp, %0" : : "r"(_current_process->stack_pointer));

    set_tick_and_enable_timer();
    if (_current_process->state == TASK_NEW) {
        _start_current_process();
    } else {
        __asm volatile("pop {r0-r12, lr}");
    }
    ENABLE_IRQ();
}

void __attribute__ ((naked)) ctx_switch(void) {
    __asm volatile("sub lr, lr, #4");
    __asm volatile("srsdb sp!, 0x13");
    __asm volatile("cps #0x13");

    __asm volatile("push {r0-r12, lr}");
    DISABLE_IRQ();

    __asm("mov %0, sp" : "=r"(_current_process->stack_pointer));
    schedule();
    __asm("mov sp, %0" : : "r"(_current_process->stack_pointer));

    set_tick_and_enable_timer();
    if (_current_process->state == TASK_NEW) {
        _start_current_process();
    } else {
        __asm volatile("pop {r0-r12, lr}");
    }
    ENABLE_IRQ();

    __asm volatile ("rfefd sp!");
}
