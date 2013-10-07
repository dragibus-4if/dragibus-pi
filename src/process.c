#include "allocateMemory.h"
#include "process.h"
/*#include <assert.h>*/

#define STACK_SIZE 128

#define PCB_FUNC_NOT_EXECUTED   0
#define PCB_FUNC_EXECUTING      1
#define PCB_FUNC_FINISHED       -1

static struct pcb_s * _first_pcb = NULL;
static struct pcb_s * _last_pcb = NULL;
static struct pcb_s * _current_pcb = NULL;

/* Initialise un PCB */
void init_pcb(struct pcb_s * pcb, func_t f, void * args, size_t stack_size) {
    /* Définie la fonction associé au PCB */
    pcb->func = f;

    /* Enregistre les paramètres de la fonction */
    pcb->args = args;

    /* Prendre l'adresse de la fonction f et la stocker dans pc */
    pcb->pc = (uint32_t) f;

    /* Initialise la stack pointer (non valide) */
    pcb->sp = 0;

    /* Indique que la fonction n'a pas été lancée */
    pcb->state = PCB_FUNC_NOT_EXECUTED;

    /* Met à jour la liste de PCB */
    if(_last_pcb != NULL) {
        _last_pcb->next_pcb = pcb;
    }
    _last_pcb = pcb;
    pcb->next_pcb = NULL;

    if(_first_pcb == NULL) {
        _first_pcb = pcb;
    }
}

void create_process(func_t f, void * args) {
    struct pcb_s * pcb = (struct pcb_s *) AllocateMemory(sizeof(struct pcb_s));
    init_pcb(pcb, f, args, STACK_SIZE);
}

/* Contexte courant */
static struct ctx_s * _current_ctx = NULL;

struct ctx_s * current_ctx(void) {
    return _current_ctx;
}

void set_current_ctx(struct ctx_s * ctx) {
    /*assert(ctx != NULL);*/
    _current_ctx = ctx;
}

void init_ctx ( struct ctx_s* ctx, func_t f, unsigned int stack_size )
{
    /* Prendre l'adresse de la fonction f et la stocker dans pc */
    ctx->pc = (uint32_t) f;

    /* Réserver de la mémoire et stocker dans sp, parce que AllMem renvoie un pointeur */
    ctx->sp = (uint32_t) AllocateMemory (stack_size) - stack_size;
}

/* Démarre une fonction en utilisant un certain contexte */
void start_ctx(struct ctx_s * ctx, func_t f, void * args) {
    /*assert(ctx != NULL);*/

    set_current_ctx(ctx);
    f(args);
}
