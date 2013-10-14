#include "sched.h"

void create_process(func_t f, void* args)
{

	/* TODO Libérer la mémoire allouée
     * Normalement celle-ci se fait lorsque le processus est fini
     */
    struct pcb_s * pcb = (struct pcb_s *) AllocateMemory(sizeof(struct pcb_s));
    _init_pcb(pcb, entry, args);

};

void yield()
{

/* TODO voir process.h pour les commentaires */
    if(_first_pcb == NULL && _last_pcb == NULL)
        return;
    if(_current_pcb != NULL) {
        /* Sauvegarde des registres dans l'ancien contexte
         * TODO push {r-1..12, lr} à faire.
         */
        __asm("push {r0-r12}");
        /* __asm("mov %0, r0"  : "=r"(_current_pcb->regs[0])); */
        /* __asm("mov %0, r1"  : "=r"(_current_pcb->regs[1])); */
        /* __asm("mov %0, r2"  : "=r"(_current_pcb->regs[2])); */
        /* __asm("mov %0, r3"  : "=r"(_current_pcb->regs[3])); */
        /* __asm("mov %0, r4"  : "=r"(_current_pcb->regs[4])); */
        /* __asm("mov %0, r5"  : "=r"(_current_pcb->regs[5])); */
        /* __asm("mov %0, r6"  : "=r"(_current_pcb->regs[6])); */
        /* __asm("mov %0, r7"  : "=r"(_current_pcb->regs[7])); */
        /* __asm("mov %0, r8"  : "=r"(_current_pcb->regs[8])); */
        /* __asm("mov %0, r9"  : "=r"(_current_pcb->regs[9])); */
        /* __asm("mov %0, r10" : "=r"(_current_pcb->regs[10])); */
        /* __asm("mov %0, r11" : "=r"(_current_pcb->regs[11])); */
        /* __asm("mov %0, r12" : "=r"(_current_pcb->regs[12])); */

        /* Sauvegarde de la valeur actuelle de SP dans l'ancien contexte */
        __asm("mov %0, sp" : "=r"(_current_pcb->sp));

        /* Sauvegarde de la valeur actuelle de PC dans l'ancien contexte */
        __asm("mov %0, pc" : "=r"(_current_pcb->pc));
    }

    _start_process(_first_pcb);

};

void start_current_process()
{




};
