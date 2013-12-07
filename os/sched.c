#include "sched.h"
#include "malloc.h"
#include "hw.h"
#include "dispatcher.h"

static struct task_struct * _sort_pcb_list(struct task_struct * pcb) {
    /* Cas de base */
    if (pcb == NULL || pcb->next == NULL) {
        return pcb;
    }

    /* Choix du point de pivot */
    static const size_t pivot_choice = 0;
    struct task_struct * pivot = pcb;
    for (size_t i = 0; i < pivot_choice && pivot->next != NULL; i++) {
        pivot = pivot->next;
    }

    /* Enlever le pivot de la liste */
    struct task_struct * next, * tmp = NULL;
    while (pcb != NULL) {
        next = pcb->next;
        if (pcb->priority != pivot->priority) {
            pcb->next = tmp;
            tmp = pcb;
        }
        pcb = next;
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
    first = _sort_pcb_list(first);
    second = _sort_pcb_list(second);

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

static struct task_struct * _schedule_priority(struct task_struct * pcb) {
    /* TODO */
    return pcb;
}

static struct task_struct _idle_process;
static struct task_struct * _ready_list = NULL;
static struct task_struct * _waiting_list = NULL;
static struct task_struct * _current_process = NULL;

/* Ordonnanceur à priority */
static enum sched_mode_e _sched_mode = BASIC;

struct task_struct * get_current_process() {
    return _current_process;
}

int set_process_state(struct task_struct * pcb, enum task_state state) {
    /* Vérif des valeurs */
    if (pcb == NULL) {
        return -1;
    }
    if (state != NEW && pcb->state == state) {
        return 0;
    }

    /* Gestion du cas particulier ou on passe de NEW à READY */
    if (pcb->state == NEW && state == READY) {
        pcb->state = READY;
        return 0;
    }

    /* On retire le pcb de sa liste de state courant (s'il en a une) */
    if(pcb->next != NULL && pcb->prev != NULL) {
        if (pcb->next == pcb) {
            /* Dans le cas de un seul élément, la liste devient vide */
            if (pcb->state == READY) {
                _ready_list = NULL;
            } else if (pcb->state == WAITING) {
                _waiting_list = NULL;
            }
        } else {
            pcb->prev->next = pcb->next;
            pcb->next->prev = pcb->prev;
        }
    }

    /* Dans le cas ou le processus est fini, on le tue */
    if (state == TERMINATED) {
        int y = pcb == _current_process;
        malloc_free((char *) pcb->stack_base);
        malloc_free((char *) pcb);
        if(y) {
            yield();
        }
        return 0;
    }

    /* On l'ajoute à la fin de sa nouvelle liste */
    /* TODO si on utilise la priorité on peut faire une
     * insertion trié ici-meme */
    if (state == READY || state == NEW) {
        if (_ready_list == NULL) {
            _ready_list = pcb;
            pcb->prev = pcb;
            pcb->next = pcb;
        } else {
            pcb->prev = _ready_list->prev;
            pcb->next = _ready_list;
            _ready_list->prev->next = pcb;
            _ready_list->prev = pcb;
        }
    } else if (state == WAITING) {
        if (_waiting_list == NULL) {
            _waiting_list = pcb;
            pcb->prev = pcb;
            pcb->next = pcb;
        } else {
            pcb->prev = _waiting_list->prev;
            pcb->next = _waiting_list;
            _waiting_list->prev->next = pcb;
            _waiting_list->prev = pcb;
        }
    }

    pcb->state = state;

    /* Dans le cas ou la tache courante est en attente, on change */
    if(pcb->state == WAITING && pcb == _current_process) {
        yield();
    }

    return 0;
}

int set_current_state(enum task_state state) {
    return set_process_state(_current_process, state);
}

void _start_current_process() {
    set_current_state(READY);
    _current_process->entry_point(_current_process->args);
    set_current_state(TERMINATED);
}

int _init_process(struct task_struct *pcb, size_t stack_size, func_t * f, void * args) {
    /* Function and args */
    pcb->entry_point = f;
    pcb->args = args;

    /* Stack allocation */
    pcb->stack_size = stack_size;
    pcb->stack_base = malloc_alloc(stack_size);
    if (pcb->stack_base == NULL) {
        return -1;
    }

    /* Priority */
    pcb->priority = 0;

    /* State and context */
    set_process_state(pcb, NEW);
    pcb->stack_pointer = ((uint32_t *) (pcb->stack_base + stack_size)) - 1;

    /* Fill in the stack with CPSR and PC */
    *(pcb->stack_pointer) = 0x53;
    pcb->stack_pointer--;
    *(pcb->stack_pointer) = (unsigned int) &_start_current_process;

    return 1;
}

int create_process(func_t * f, void * args, size_t size) {
    struct task_struct * pcb;
    pcb = (struct task_struct *) malloc_alloc(sizeof(struct task_struct));
    if (pcb == NULL) {
        return -1;
    }

    pcb->next = NULL;
    pcb->prev = NULL;

    return _init_process(pcb, size, f, args);
}

void schedule() {
    /* Si on a des process prets */
    if (_ready_list != NULL) {
        /* Rajout de ce pcb à la fin de la liste */
        _current_process = _ready_list;
        _ready_list = _ready_list->next;
    } else {
        /* Sinon on utilise le pcb idle */
        _current_process = &_idle_process;
    }
}

void start_sched() {
    _idle_process.prev = NULL;
    _idle_process.next = NULL;
    _current_process = &_idle_process;
    yield();
    while (1);
}

void yield() {
    __asm volatile("push {r0-r12, lr}");
    DISABLE_IRQ();

    __asm("mov %0, sp" : "=r"(_current_process->stack_pointer));
    schedule();
    __asm("mov sp, %0" : : "r"(_current_process->stack_pointer));

    set_tick_and_enable_timer();
    if (_current_process->state == NEW) {
        _start_current_process();
    } else {
        __asm volatile("pop {r0-r12, lr}");
    }
}
