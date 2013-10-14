#include "dispatcher.h"

void switch_to(struct ctx_s * ctx)
{
    /* Sauvegarde de la valeur actuelle de SP dans l'ancien contexte */
    __asm("mov %0, sp" : "=r"(current.ctx->sp));

    /* Sauvegarde de la valeur actuelle de PC dans l'ancien contexte */
    __asm("mov %0, lr" : "=r"(current.ctx->pc));

    /* Sauvegarde des registres */
    __asm("mov %0, r0" : "=r"(current.ctx->regs[0]));
    __asm("mov %0, r1" : "=r"(current.ctx->regs[1]));
    __asm("mov %0, r2" : "=r"(current.ctx->regs[2]));
    __asm("mov %0, r3" : "=r"(current.ctx->regs[3]));
    __asm("mov %0, r4" : "=r"(current.ctx->regs[4]));
    __asm("mov %0, r5" : "=r"(current.ctx->regs[5]));
    __asm("mov %0, r6" : "=r"(current.ctx->regs[6]));
    __asm("mov %0, r7" : "=r"(current.ctx->regs[7]));
    __asm("mov %0, r8" : "=r"(current.ctx->regs[8]));
    __asm("mov %0, r9" : "=r"(current.ctx->regs[9]));
    __asm("mov %0, r10" : "=r"(current.ctx->regs[10]));
    __asm("mov %0, r11" : "=r"(current.ctx->regs[11]));
    /* TODO */

    /* Changement de contexte */
    current.ctx = ctx;

    /* Chargement de la valeur de SP stockée dans le nouveau contexte */
    __asm("mov sp, %0" : : "r"(current.ctx->sp));

    /* Chargement de la valeur de PC stockée dans le nouveau contexte */
    __asm("mov lr, %0" : : "r"(current.ctx->pc));

    /* Chargement des registres */
    __asm("mov r0, %0" : : "r"(current.ctx->regs[0]));
    __asm("mov r1, %0" : : "r"(current.ctx->regs[1]));
    __asm("mov r2, %0" : : "r"(current.ctx->regs[2]));
    __asm("mov r3, %0" : : "r"(current.ctx->regs[3]));
    __asm("mov r4, %0" : : "r"(current.ctx->regs[4]));
    __asm("mov r5, %0" : : "r"(current.ctx->regs[5]));
    __asm("mov r6, %0" : : "r"(current.ctx->regs[6]));
    __asm("mov r7, %0" : : "r"(current.ctx->regs[7]));
    __asm("mov r8, %0" : : "r"(current.ctx->regs[8]));
    __asm("mov r9, %0" : : "r"(current.ctx->regs[9]));
    __asm("mov r10, %0" : : "r"(current.ctx->regs[10]));
    __asm("mov r11, %0" : : "r"(current.ctx->regs[11]));
    /* TODO */

    return;
}

/* Pour sauvegarder le contexte:
 * push {lr}
 * sub sp, sp, #12 + 4 * sizeof(args)
 * str r0, [sp, #4]
 * ldr r3, [sp, #4]
 * mov rtruc, args
 * bl foo
 * str r0 [sp, #4]
 * ldr r3, [sp, #4]
 * mov r0, r3
 * add sp, sp, #12
 * pop {pc}
 */

/* Autrement dit: */
/* __asm("mov %%sp, %0
   mov %%pc, %1
   mov %%r0, %2
   mov %%r1, %3
   mov %%r2, %4
   mov %%r3, %5
   mov %%r4, %6
   mov %%r5, %7
   mov %%r6, %8
   mov %%r7, %9
   mov %%r8, %10
   mov %%r9, %11
   mov %%r10, %12
   mov %%r11, %13
   mov %%r12, %14
   mov %%r13, %15
   mov %%r14, %16
   mov %%r15, %17
   bru %%pc"
   :
   : "r"(ctx->sp),
   "r"(ctx->pc),
   "r"(ctx->regs[0]),
   "r"(ctx->regs[1]),
   "r"(ctx->regs[2]),
   "r"(ctx->regs[3]),
   "r"(ctx->regs[4]),
   "r"(ctx->regs[5]),
   "r"(ctx->regs[6]),
   "r"(ctx->regs[7]),
   "r"(ctx->regs[8]),
   "r"(ctx->regs[9]),
   "r"(ctx->regs[10]),
   "r"(ctx->regs[11]),
   "r"(ctx->regs[12]),
   "r"(ctx->regs[13]),
   "r"(ctx->regs[14]),
   "r"(ctx->regs[15]),
   : "%sp",
   "%pc",
   "%r0",
   "%r1",
   "%r2",
   "%r3",
   "%r4",
   "%r5",
   "%r6",
   "%r7",
   "%r8",
   "%r9",
   "%r10",
   "%r11",
   "%r12",
   "%r13",
   "%r14",
   "%r15");
   */
/* } */
