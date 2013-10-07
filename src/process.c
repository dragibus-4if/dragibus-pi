#include "allocateMemory.h"
#include "process.h"
#include <assert.h>

/* Contexte courant */
static struct ctx_s * _current_ctx = NULL;

struct ctx_s * current_ctx(void) {
    return _current_ctx;
}

void set_current_ctx(struct ctx_s * ctx) {
    assert(ctx != NULL);
    _current_ctx = ctx;
}

void init_ctx ( struct ctx_s* ctx, func_t f, unsigned int stack_size )
{
    /*prendre l'adresse de la fonction f et la stocker dans pc*/
    ctx->pc = (uint32_t) f;

    /*réserver de la mémoire et stocker dans sp, parce que AllMem renvoie un pointeur*/
    ctx->sp = (uint32_t) AllocateMemory (stack_size) - stack_size;

    return;
}

/* Démarre une fonction en utilisant un certain contexte */
void start_ctx(struct ctx_s * ctx, func_t f, void * args) {
    assert(ctx != NULL);

    /* _current_ctx = ctx; */
    current.ctx = ctx;
    /*f(args);*/
    f();
}
