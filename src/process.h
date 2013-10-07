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

/* Définition d'un Process Control Block (PCB)
 * func    : Pointeur vers la fonction à appeler
 * args     : Argument à donners à la fonction
 * pc       : "Program Counter", adresse courante de l'exécution
 * sp       : "Stack Pointer", pointeur de pile
 * regs     : Liste des registres (r0..r12)
 * state    : 0 si la fonction n'a pas été lancé. 1 si elle l'est. -1 si elle est finie.
 * next_pcb : PCB suivant à exécuter
 */
struct pcb_s {
    func_t func;
    void * args;

    uint32_t pc;
    uint32_t sp;
    int32_t regs[12];

    int8_t state;
    struct pcb_s * next_pcb;
};

extern void init_ctx(struct ctx_s * ctx, func_t f, unsigned int stack_size);

extern void start_ctx(struct ctx_s * ctx, func_t f, void * args);

/* Accesseurs pour le contexte courant (USE AT YOUR OWN RISK) */
extern struct ctx_s * current_ctx(void);
extern void set_current_ctx(struct ctx_s *);

/* Crée un process */
extern void create_process(func_t f, void * args);

extern void yield();

extern void start_current_process();

extern void ctx_switch();

#endif
