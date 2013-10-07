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

extern void init_ctx(struct ctx_s * ctx, func_t f, unsigned int stack_size);

extern void start_ctx(struct ctx_s * ctx, func_t f, void * args);

/* Accesseurs pour le contexte courant (USE AT YOUR OWN RISK) */
extern struct ctx_s * current_ctx(void);
extern void set_current_ctx(struct ctx_s *);

#endif
