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

/* Définition d'un Process Control Block (PCB)
 * funct    : Pointeur vers la fonction à appeler
 * args     : Argument à donners à la fonction
 * pc       : "Program Counter", adresse courante de l'exécution
 * sp       : "Stack Pointer", pointeur de pile
 * regs     : Liste des registres (r0..r15)
 * state    : 0 si la fonction n'a pas été lancé. 1 si elle l'est. -1 si elle est arreté.
 */
struct pcb_s {
    funct_t funct;
    void * args;

    uint32_t pc;
    uint32_t sp;
    int32_t regs[15];
};

void init_ctx ( struct ctx_s* ctx, func_t f, unsigned int stack_size );

void start_ctx ( struct ctx_s* ctx, func_t f, void * args );

/* Initialise un PCB */
extern void init_pcb();

extern void create_process(funct_t f, void args);

extern void yield();

extern void start_current_process();

extern void ctx_switch();

#endif
