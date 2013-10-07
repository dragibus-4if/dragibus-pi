#include "dispatcher.h"

/*Définitions de fonctions locales*/
void switch_to(struct ctx_s* ctx)
{
    /*Sauvegarde de la valeur actuelle de SP dans l'ancien contexte*/
    __asm("mov %0, sp" : "=r"(current.ctx->sp));

    /*Sauvegarde de la valeur actuelle de PC dans l'ancien contexte*/
    __asm("mov %0, lr" : "=r"(current.ctx->pc));

    /*Sauvegarde des registres*/

    /*Changement de contexte*/
    current.ctx = ctx;

    /*Chargement de la valeur de SP stockée dans le nouveau contexte*/
    __asm("mov sp, %0" : : "r"(current.ctx->sp));

    /*Chargement de la valeur de PC stockée dans le nouveau contexte*/
    __asm("mov lr, %0" : : "r"(current.ctx->pc));

    /*Chargement des registres*/
    /* TODO */

    return;
}
