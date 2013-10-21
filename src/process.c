#include "allocateMemory.h"
#include "process.h"

#define STACK_SIZE 128

#define PCB_FUNC_NOT_EXECUTED   0
#define PCB_FUNC_EXECUTING      1
#define PCB_FUNC_FINISHED       -1

/* \brief Définition d'un Process Control Block (PCB)
 * \var entry       Pointeur vers la fonction d'entrée à appeler
 * \var args        Argument à donners à la fonction
 * \var pc          "Program Counter", adresse courante de l'exécution
 * \var sp          "Stack Pointer", pointeur de pile
 * \var state       Etat d'exécution du processus
 * \var next_pcb    PCB suivant à exécuter
 *
 * Représentent les informations et le contexte d'un processus. Le contexte est
 * stocké dans le Program Counter (PC) qui représente l'adresse mémoire de code
 * que le processeur est en train d'exécuter. Au début d'exécution du
 * processus, celui-ci est placé au niveau du pointeur de fonction. A un
 * changement de contexte, cette adresse est stocké pour pouvoir y retourner
 * quand ce processus se réactive. Le Stack Pointer (SP) représente le niveau
 * d'utilisation de la pile. Au début d'exécution du processus, celui-ci est
 * initialisé à la valeur courante de l'état de la pile. A un changement de
 * contexte, il est sauvegardé avant de basculer à un autre processus. Il est
 * remit en état lorsque ce processus est réactivé. Il en est de meme pour les
 * registres d'exécution R0 à R12. Le processus est représenté par une fonction
 * d'entrée prenant un argument ainsi qu'un état. Initialement, cet état
 * indique que le processus n'a jamais été lancé. Elle est mis à jour pour
 * indiquer que le processus est en état d'exécution pendant le démarrage de
 * celui-ci. Lorsque la fonction d'entrée est finie, l'état du processus
 * l'indique pour que l'ordonnanceur nettoie et retire le processus. Les
 * processus forment une liste chaînée pour que l'ordonnanceur puisse les
 * garder en mémoire et naviguer de l'un à l'autre.
 */
struct pcb_s {
    func_t entry;
    void * args;

    uint32_t pc;
    uint32_t sp;

    int8_t state;
    struct pcb_s * next_pcb;
};

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
static void _start_process(struct pcb_s * pcb);

/* \brief Ferme le processus du PCB courant
 *
 * Se charge d'intercepter les signaux de fin des processus en se placant
 * constamment dans le registre LR (via _start_process et _ctx_switch). Libère
 * la mémoire utilisé par le processus courant puis exécute un autre processus
 * en appelant yield.
 *
 * Dans le cas où il n'y a pas de processus en court d'utilisation, cette
 * fonction ne fait rien.
 */
static void _close_current_pcb();

/* TODO commentaire complet */
static void _ctx_switch(struct pcb_s * pcb);

/* Premier élément de la liste de PCB */
static struct pcb_s * _first_pcb    = NULL;

/* Dernier élément de la liste de PCB */
static struct pcb_s * _last_pcb     = NULL;

/* Pointeur vers le PCB courant */
static struct pcb_s * _current_pcb  = NULL;

/****************************************************************************/
/************************ DEFINITION DES FONCTIONS PRIVEE *******************/
/****************************************************************************/

static void _init_pcb(struct pcb_s * pcb, func_t entry, void * args) {
    pcb->entry  = entry;
    pcb->args   = args;
    pcb->pc     = (uint32_t) pcb->entry;
    pcb->state  = PCB_FUNC_NOT_EXECUTED;
    pcb->sp     = (uint32_t) AllocateMemory(STACK_SIZE * sizeof(uint32_t));

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

static void _start_current_process() {
    /* Ne rien faire si le pcb n'est pas défini. */
    if (_current_process == NULL) {
        return;
    }

    /* Ne rien faire si le pcb a déjà été lancé. */
    if (_current_process->state != PCB_FUNC_NOT_EXECUTED) {
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

static void _ctx_switch() {
    if(_current_pcb == NULL)
        return;

    /* Chargement des registres */
    __asm("pop {lr r0-r12}");
}

/****************************************************************************/
/*********************** DEFINITION DES FONCTIONS PUBLIQUE ******************/
/****************************************************************************/

void create_process(func_t entry, void * args) {
    struct pcb_s * pcb = (struct pcb_s *) AllocateMemory(sizeof(struct pcb_s));
    _init_pcb(pcb, entry, args);
}

void yield() {
    /* Stockage des registres avant de les utiliser */
    __asm("push {lr r0-r12}");

    if(_first_pcb == NULL && _last_pcb == NULL) {
        __asm("pop {lr r0-r12}");
        return;
    }
    if(_current_pcb != NULL) {
        /* Sauvegarde de la valeur actuelle de SP dans l'ancien contexte */
        __asm("mov %0, sp" : "=r"(_current_pcb->sp));
    }

    /* TODO Placer ce choix dans schedule.c */
    _current_pcb = _current_pcb->next_pcb;

    if(_current_pcb->state == PCB_FUNC_NOT_EXECUTED) {
        __asm("pop {lr r0-r12}");
        _start_process(_current_pcb);
    }
    else {
        _ctx_switch(_current_pcb);
    }
}

/****************************************************************************/
/******************** ANCIEN CODE POUR L'ORDONNANCEUR SIMPLE ****************/
/****************************************************************************/

/* Contexte courant */
/* static struct ctx_s * _current_ctx = NULL; */

/* struct ctx_s * current_ctx(void) { */
/*     return _current_ctx; */
/* } */

/* void set_current_ctx(struct ctx_s * ctx) { */
/*     _current_ctx = ctx; */
/* } */

void init_ctx(struct ctx_s* ctx, func_t f, size_t stack_size) {
    /* Prendre l'adresse de la fonction f et la stocker dans pc */
    ctx->pc = (uint32_t) f;

    /* Réserver de la mémoire et stocker dans sp, parce que AllMem renvoie un pointeur */

    uint32_t* stack_base = (uint32_t*) AllocateMemory(stack_size);
    ctx->sp = stack_base + stack_size -1; 

    return;
}

/* Démarre une fonction en utilisant un certain contexte */
void start_ctx(struct ctx_s * ctx, func_t f) {
    current_ctx = ctx;
    f();
}
