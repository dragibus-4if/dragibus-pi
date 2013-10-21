#include "allocateMemory.h"
#include "process.h"

/* \brief Initialise un PCB
 * \param pcb   Pointeur vers le bloc à initialiser
 * \param entry Pointeur vers la fonction d'entrée du processus
 * \param args  Paramètre de la fonction d'entrée
 *
 * Initialise le bloc de processus donné en préparant le contexte enregistré
 * en plaçant PC à l'adresse de la fonction d'entrée. Les registres SP et R0 à
 * R12 ne sont pas définis et contiennent donc pour le moment des valeurs
 * invalides. Ces valeurs seront mis à jour au premier lancement du processus.
 * L'état du processus indique alors qu'il n'a jamais été lancé. Le PCB est
 * également ajouté à la liste chaîné des PCBs utilisés par l'ordonnanceur.
 */
static void _init_pcb(struct pcb_s * pcb, func_t entry, void * args);

struct pcb_s * _first_pcb   = NULL;
struct pcb_s * _last_pcb    = NULL;
struct pcb_s * _current_pcb = NULL;

void _init_pcb(struct pcb_s * pcb, func_t entry, void * args) {
    pcb->entry  = entry;
    pcb->args   = args;
    pcb->pc     = (uint32_t) pcb->entry;
    pcb->state  = PCB_FUNC_NOT_EXECUTED;
    pcb->sp     = (uint32_t) AllocateMemory(STACK_SIZE) + STACK_SIZE - 1;

    /* Ajoute le PCB à la fin de la liste */
    /* Si la liste est vide */
    if(_first_pcb == NULL && _last_pcb == NULL) {
        _first_pcb = pcb;
        _last_pcb = pcb;
    }
    else {
        _last_pcb->next_pcb = pcb;
        _last_pcb = pcb;
    }
    pcb->next_pcb = _first_pcb;
}

void create_process(func_t entry, void * args) {
    struct pcb_s * pcb = (struct pcb_s *) AllocateMemory(sizeof(struct pcb_s));
    _init_pcb(pcb, entry, args);
}

