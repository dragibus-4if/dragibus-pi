#ifndef PROCESS_H
#define PROCESS_H

#include "types.h"

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

/* Initialise un contexte */
extern void init_ctx(struct ctx_s * ctx, func_t f, size_t stack_size);

/* Démarre une fonction en utilisant un certain contexte */
extern void start_ctx(struct ctx_s * ctx, func_t f);

/* Switch à un autre contexte */
extern void switch_to(struct ctx_s * ctx);

#endif
