#include "sched.h"

void create_process(func_t f, void* args)
{
    /* TODO Libérer la mémoire allouée
     * Normalement celle-ci se fait lorsque le processus est fini
     */
    struct pcb_s * pcb = (struct pcb_s *) AllocateMemory(sizeof(struct pcb_s));
    _init_pcb(pcb, entry, args);
}

/* TODO voir process.h pour les commentaires */
void yield()
{
    if (_first_pcb == NULL && _last_pcb == NULL) {
        return;
    }

    if (_current_pcb != NULL) {
        /* Sauvegarde des registres dans l'ancien contexte
         * TODO push {r-1..12, lr} à faire.
         */
        __asm("push {r0-r12}");

        /* Sauvegarde de la valeur actuelle de SP dans l'ancien contexte */
        __asm("mov %0, sp" : "=r"(_current_pcb->sp));

        /* Sauvegarde de la valeur actuelle de PC dans l'ancien contexte */
        __asm("mov %0, pc" : "=r"(_current_pcb->pc));
    }
    _start_process(_first_pcb);
}

void start_current_process()
{
}
