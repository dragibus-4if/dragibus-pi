#include "sched.h"

/* \brief Exécute une première fois un processus
 * \param pcb   Bloc d'infomation du processus à commencer
 *
 * Exécute pour une première fois seulement un processus. Les informations le
 * concernant et n'étant toujours pas valide (comme la pile et les registres)
 * sont récupérées et l'état du processus est alors en cours d'exécution. Un
 * changement de contexte a alors lieu pour appeler ce processus.
 *
 * Si le processus a déjà été initialisé, la fonction ne fait rien.
 * Dans le cas ou aucun processus n'a pas encore été lancé,
 * la stack n'a toujours pas été faite.
 * Elle est alors alloué et l'adresse correspond au niveau
 * initial de la stack.
 */
static void _start_current_process();

/* \brief Ferme le processus du PCB courant
 *
 * Se charge d'intercepter les signaux de fin des processus en se placant
 * constamment dans le registre LR (via _start_current_process et _ctx_switch). Libère
 * la mémoire utilisé par le processus courant puis exécute un autre processus
 * en appelant yield.
 *
 * Dans le cas où il n'y a pas de processus en court d'utilisation, cette
 * fonction ne fait rien.
 */
static void _close_current_pcb();

/* TODO commentaire complet */
static void _ctx_switch();

void yield() {
    /* Stockage des registres avant de les utiliser */
    __asm("push {lr, r0-r12}");

    if(_first_pcb == NULL && _last_pcb == NULL) {
        __asm("pop {lr, r0-r12}");
        return;
    }
    if(_current_pcb != NULL) {
        /* Sauvegarde de la valeur actuelle de SP dans l'ancien contexte */
        __asm("mov %0, sp" : "=r"(_current_pcb->sp));
    }

    /* TODO Placer ce choix dans schedule.c */
    _current_pcb = _current_pcb->next_pcb;

    if(_current_pcb->state == PCB_FUNC_NOT_EXECUTED) {
        __asm("pop {lr, r0-r12}");
        _start_current_process();
    }
    else {
        _ctx_switch();
    }
}

static void _ctx_switch() {
    if(_current_pcb == NULL)
        return;

    /* Chargement des registres */
    __asm("pop {lr, r0-r12}");
}

static void _start_current_process() {
    /* Ne rien faire si le pcb n'est pas défini. */
    if (_current_pcb == NULL) {
        return;
    }

    /* Ne rien faire si le pcb a déjà été lancé. */
    if (_current_pcb->state != PCB_FUNC_NOT_EXECUTED) {
        return;
    }

    /* Appel de la procédure */
    _current_pcb->entry(_current_pcb->args);

    /* Ferme le processus */
    _close_current_pcb();
}

static void _close_current_pcb() {
    if(_current_pcb == NULL)
        return;

    _current_pcb->state = PCB_FUNC_FINISHED;

    FreeAllocatedMemory((uint32_t *) _current_pcb->sp);
    FreeAllocatedMemory((uint32_t *) _current_pcb);

    yield();
}

