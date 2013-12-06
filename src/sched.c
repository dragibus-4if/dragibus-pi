#include "sched.h"
#include "malloc.h"
#include "hw.h"
#include "dispatcher.h"

/* Quicksort */
/*
function quicksort(array)
    if length(array) ≤ 1
        return array  // an array of zero or one elements is already sorted
    select and remove a pivot element pivot from 'array'  // see '#Choice of pivot' below
    create empty lists less and greater
    for each x in array
        if x ≤ pivot then append x to less'
        else append x to greater
    return concatenate(quicksort(less), list(pivot), quicksort(greater)) // two recursive calls
*/
/* TODO */

/* Fonction de comparaison des pcb selon la priorité (inversée). Retourne 0 si
 * x est supérieur à y, -1 sinon. */
static int _compare_pcbs(void * x, void * y) {
    struct pcb_s * ox = ((struct pcb_s *) x);
    struct pcb_s * oy = ((struct pcb_s *) y);
    if (ox->priority < oy->priority) {
        return -1;
    } if (ox->priority > oy->priority) {
        return 1;
    } else {
        return 0;
    }
}

static struct pcb_s _idle;
static struct pcb_s * _ready_queue = NULL;
static struct pcb_s * _current_process = NULL;
static struct pcb_s * _waiting_queue = NULL;

/* Ordonnanceur à priority */
static enum sched_mode_e _sched_mode = BASIC;

/* Implémentation publique de l'ordonnanceur */

struct pcb_s * get_current_process() {
    return _current_process;
}

void process_block() {
    struct pcb_s * it = _current_process;

    while (it->next != _current_process) {
        it = it->next;
    }

    it->next = it->next->next;

    it = _waiting_queue;
    while (it->next != NULL) {
        it = it->next;
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
    _current_process->entry_point();

    /* The process is terminated */
    _current_process->state = TERMINATED;
    yield();
}

static int _init_process(struct pcb_s * pcb, size_t stack_size, func_t * f) {
    /* Function and args */
    pcb->entry_point = f;

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
    pcb->sp --;
    *(pcb->sp) = (unsigned int) &start_current_process;

    return 1;
}

int create_process(func_t * f, size_t size) {
    struct pcb_s * pcb;
    pcb = (struct pcb_s *) malloc_alloc(sizeof(struct pcb_s));
    if (pcb == NULL) {
        return 0;
    }

    /* First process */
    if (! _ready_queue) {
        _ready_queue = pcb;
    } else {
        pcb->next = _ready_queue->next;
    }

    _ready_queue->next = pcb;
    return _init_process(pcb,size,f);
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

    /*ENABLE_IRQ();*/
    while (1) {
        yield();
    }
}

void yield() {
    ctx_switch();
}
