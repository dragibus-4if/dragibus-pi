#ifndef PROCESS_H
#define PROCESS_H

#include "types.h"





/*Variables globales*/
struct current_s
{
	struct ctx_s* ctx;

} current;

/* Structure de donnée définissant un contexte:
 *  pc : "program counter", adresse courante de l'exécution
 *  sp : "stack pointer", pointeur de pile
 *  regs : données des registres (r0..15)
 */
struct ctx_s {
    uint32_t pc;
    uint32_t sp;
    uint32_t regs[15];

};


void init_ctx ( struct ctx_s* ctx, func_t f, unsigned int stack_size );

void start_ctx ( struct ctx_s* ctx, func_t f, void * args );

#endif
