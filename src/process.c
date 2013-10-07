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

    current_context = ctx;
    f();

    /* push {lr} */
    /* sub sp, sp, #12 + 4 * sizeof(args) */
    /* str r0, [sp, #4] */
    /* ldr r3, [sp, #4] */
    /* mov rtruc, args */
    /* bl foo */
    /* str r0 [sp, #4] */
    /* ldr r3, [sp, #4] */
    /* mov r0, r3 */
    /* add sp, sp, #12 */
    /* pop {pc} */

    /* __asm("mov %%sp, %0 */
    /*         mov %%pc, %1 */
    /*         mov %%r0, %2 */
    /*         mov %%r1, %3 */
    /*         mov %%r2, %4 */
    /*         mov %%r3, %5 */
    /*         mov %%r4, %6 */
    /*         mov %%r5, %7 */
    /*         mov %%r6, %8 */
    /*         mov %%r7, %9 */
    /*         mov %%r8, %10 */
    /*         mov %%r9, %11 */
    /*         mov %%r10, %12 */
    /*         mov %%r11, %13 */
    /*         mov %%r12, %14 */
    /*         mov %%r13, %15 */
    /*         mov %%r14, %16 */
    /*         mov %%r15, %17 */
    /*         bru %%pc" */
    /*         : */
    /*         : "r"(ctx->sp), */
    /*         "r"(ctx->pc), */
    /*         "r"(ctx->regs[0]), */
    /*         "r"(ctx->regs[1]), */
    /*         "r"(ctx->regs[2]), */
    /*         "r"(ctx->regs[3]), */
    /*         "r"(ctx->regs[4]), */
    /*         "r"(ctx->regs[5]), */
    /*         "r"(ctx->regs[6]), */
    /*         "r"(ctx->regs[7]), */
    /*         "r"(ctx->regs[8]), */
    /*         "r"(ctx->regs[9]), */
    /*         "r"(ctx->regs[10]), */
    /*         "r"(ctx->regs[11]), */
    /*         "r"(ctx->regs[12]), */
    /*         "r"(ctx->regs[13]), */
    /*         "r"(ctx->regs[14]), */
    /*         "r"(ctx->regs[15]), */
    /*         : "%sp", */
    /*         "%pc", */
    /*         "%r0", */
    /*         "%r1", */
    /*         "%r2", */
    /*         "%r3", */
    /*         "%r4", */
    /*         "%r5", */
    /*         "%r6", */
    /*         "%r7", */
    /*         "%r8", */
    /*         "%r9", */
    /*         "%r10", */
    /*         "%r11", */
    /*         "%r12", */
    /*         "%r13", */
    /*         "%r14", */
    /*         "%r15"); */
    /* current_context = ctx; */
    /* f(); */
}

/* Switch à un autre contexte */
void switch_to(struct ctx_s * ctx) {
}
