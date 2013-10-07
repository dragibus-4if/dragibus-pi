#include "process.h"
#include "allocateMemory.h"

/* Contexte courant */
static struct ctx_s * current_context = 0;

/* Initialise un contexte */
void init_ctx(struct ctx_s * ctx, func_t f, size_t stack_size) {
    ctx->pc = f;
    ctx->sp = AllocateMemory( stack_size ) + stack_size ;
}

/* Démarre une fonction en utilisant un certain contexte */
void start_ctx(struct ctx_s * ctx, func_t f) {
    if(ctx == NULL)
        return;

    __asm("mov %%sp, %0" "\n\t"
            "mov %%pc, %1"
            :
            : "r"(ctx->sp), "r"(ctx->pc)
            : "%sp", "%pc");
    current_context = ctx;
    f();
}

/* Switch à un autre contexte */
void switch_to(struct ctx_s * ctx) {
}
