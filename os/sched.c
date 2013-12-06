#include "sched.h"
#include "malloc.h"
#include "hw.h"
#include "dispatcher.h"

static struct pcb_s * _sort_pcb_list(struct pcb_s * pcb) {
    /* Cas de base */
    if (pcb == NULL || pcb->next == NULL) {
        return pcb;
    }

    /* Choix du point de pivot */
    static const size_t pivot_choice = 0;
    struct pcb_s * pivot = pcb;
    for (size_t i = 0; i < pivot_choice && pivot->next != NULL; i++) {
        pivot = pivot->next;
    }

    /* Enlever le pivot de la liste */
    struct pcb_s * next, * tmp = NULL;
    while (pcb != NULL) {
        next = pcb->next;
        if (pcb->priority != pivot->priority) {
            pcb->next = tmp;
            tmp = pcb;
        }
        pcb = next;
    }

    /* Divide & conquer */
    struct pcb_s * first = NULL, * second = NULL;
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
        struct pcb_s * end = first;
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

static struct pcb_s * _schedule_priority(struct pcb_s * pcb) {
    /* TODO */
    return pcb;
}

static struct pcb_s _idle;
static struct pcb_s * _ready_queue = NULL;
static struct pcb_s * _current_process = NULL;
static struct pcb_s * _waiting_queue = NULL;

/* Ordonnanceur à priority */
static enum sched_mode_e _sched_mode = BASIC;

struct pcb_s * get_current_process() {
    return _current_process;
}

void process_block() {
    struct pcb_s * it = _current_process;
    while (it->next != _current_process) {
        it = it->next;
    }

    it->next = it->next->next;

    /* TODO Ta waiting queue elle boucle pas sur elle-meme */
    /* Ca fait du caca de partout */
    it = _waiting_queue;
    if (_waiting_queue == 0 )
    {
        _waiting_queue = _current_process;
    } else {
        while(it->next != _waiting_queue) {
            it = it->next;
        }
    }


    it->next = _current_process;
    _current_process->next = NULL;
    _current_process->state = WAITING;
}

void process_release(struct pcb_s * pcb) {
    struct pcb_s * it = _waiting_queue;
    while (it->next != pcb) {
        it = it->next;
    }

    it->next = it->next->next;

    pcb->next = _current_process->next;
    _current_process->next = pcb;
    pcb->state = READY;
}

void start_current_process() {
    _current_process->state = READY;
    _current_process->entry_point(_current_process->args);

    /* The process is terminated */
    _current_process->state = TERMINATED;
    yield();
}

int _init_process(struct pcb_s *pcb, size_t stack_size, func_t * f, void * args) {
    /* Function and args */
    pcb->entry_point = f;
    pcb->args = args;

    /* Stack allocation */
    pcb->size = stack_size;
    pcb->stack_base = malloc_alloc(stack_size);
    if (pcb->stack_base == NULL) {
        return 0;
    }

    /* Priority */
    pcb->priority = 0;

    /* State and context */
    pcb->state = NEW;
    pcb->sp = ((uint32_t *) (pcb->stack_base + stack_size)) - 1;

    /* Fill in the stack with CPSR and PC */
    *(pcb->sp) = 0x53;
    pcb->sp--;
    *(pcb->sp) = (unsigned int) &start_current_process;

    return 1;
}

int create_process(func_t * f, void * args, size_t size) {
    struct pcb_s * pcb;
    pcb = (struct pcb_s *) malloc_alloc(sizeof(struct pcb_s));
    if (pcb == NULL) {
        return 0;
    }

    /* First process */
    if (_ready_queue == NULL) {
        _ready_queue = pcb;
    } else {
        pcb->next = _ready_queue->next;
    }

    _ready_queue->next = pcb;
    return _init_process(pcb, size, f, args);
}

void schedule() {
    struct pcb_s * pcb;
    struct pcb_s * pcb_init;

    pcb_init = _current_process;
    pcb = _current_process;

    /* Start by eliminating all zombies (rhaaaaa !) */
    while (pcb->next->state == TERMINATED) {
        /* If no process to be executed -> note that and leave loop */
        if (pcb->next == pcb) {
            pcb = NULL;
            break;
        } else {
            /* Particular case of the head */
            if (pcb->next == _ready_queue) {
                _ready_queue = pcb;
            }

            /* Remove pcb from the list (FIXME could be done after the loop) */
            pcb->next = pcb->next->next;

            /* Free memory */
            malloc_free((char *) pcb->next->stack_base);
            malloc_free((char *) pcb->next);

            /* Get next process */
            pcb = pcb->next;
        }
    }

    if (pcb != NULL) {
        /* On parcours la liste jusqu'à trouver un processus non bloqué */
        pcb = pcb->next;
        while (pcb->state == WAITING && pcb != pcb_init) {
            pcb = pcb->next;
        }

        /* Si tous les processus sont bloqués -> on le note */
        if (pcb->state == WAITING) {
            pcb = NULL;
        }
    }

    /* Application de l'ordonnanceur à priorité */
    if (_sched_mode == PRIORITY) {
        pcb = _schedule_priority(pcb);
    }

    if (pcb == NULL) {  /* Si pas de processus à élire -> _idle */
        _ready_queue = NULL;
        _current_process = &_idle;
    } else {            /* Sinon -> le processus élu est le suivant */
        _current_process = pcb;
    }
}

void start_sched() {
    _current_process = &_idle;
    _idle.next = _ready_queue;
    ENABLE_IRQ();
    while (1) {
        yield();
    }
}

void yield() {
    ctx_switch();
}
