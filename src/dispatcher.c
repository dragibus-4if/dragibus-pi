#include "dispatcher.h"

void switch_to(struct ctx_s * ctx)
{
    /* Sauvegarde des registres */
    __asm("mov %0, r0"  : "=r"(current_ctx->regs[0]));
    __asm("mov %0, r1"  : "=r"(current_ctx->regs[1]));
    __asm("mov %0, r2"  : "=r"(current_ctx->regs[2]));
    __asm("mov %0, r3"  : "=r"(current_ctx->regs[3]));
    __asm("mov %0, r4"  : "=r"(current_ctx->regs[4]));
    __asm("mov %0, r5"  : "=r"(current_ctx->regs[5]));
    __asm("mov %0, r6"  : "=r"(current_ctx->regs[6]));
    __asm("mov %0, r7"  : "=r"(current_ctx->regs[7]));
    __asm("mov %0, r8"  : "=r"(current_ctx->regs[8]));
    __asm("mov %0, r9"  : "=r"(current_ctx->regs[9]));
    __asm("mov %0, r10" : "=r"(current_ctx->regs[10]));
    __asm("mov %0, r11" : "=r"(current_ctx->regs[11]));
    __asm("mov %0, r12" : "=r"(current_ctx->regs[12]));
    /* TODO */

        /* Sauvegarde de la valeur actuelle de SP dans l'ancien contexte */
    __asm("mov %0, sp" : "=r"(current_ctx->sp));

    /* Sauvegarde de la valeur actuelle de PC dans l'ancien contexte */
    __asm("mov %0, lr" : "=r"(current_ctx->pc));

    /* Changement de contexte */
    current_ctx = ctx;

    /* Chargement de la valeur de SP stockée dans le nouveau contexte */
    __asm("mov sp, %0" : : "r"(current_ctx->sp));

    /* Chargement de la valeur de PC stockée dans le nouveau contexte */
    __asm("mov lr, %0" : : "r"(current_ctx->pc));

    /* Chargement des registres */
    __asm("mov r0, %0"  : : "r"(current_ctx->regs[0]));
    __asm("mov r1, %0"  : : "r"(current_ctx->regs[1]));
    __asm("mov r2, %0"  : : "r"(current_ctx->regs[2]));
    __asm("mov r3, %0"  : : "r"(current_ctx->regs[3]));
    __asm("mov r4, %0"  : : "r"(current_ctx->regs[4]));
    __asm("mov r5, %0"  : : "r"(current_ctx->regs[5]));
    __asm("mov r6, %0"  : : "r"(current_ctx->regs[6]));
    __asm("mov r7, %0"  : : "r"(current_ctx->regs[7]));
    __asm("mov r8, %0"  : : "r"(current_ctx->regs[8]));
    __asm("mov r9, %0"  : : "r"(current_ctx->regs[9]));
    __asm("mov r10, %0" : : "r"(current_ctx->regs[10]));
    __asm("mov r11, %0" : : "r"(current_ctx->regs[11]));
    __asm("mov r12, %0" : : "r"(current_ctx->regs[12]));

    /* TODO */

    return;
}

