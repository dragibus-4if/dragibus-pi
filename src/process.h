#ifndef PROCESS_H
#define PROCESS_H

#include "types.h"

#define STACK_SIZE 128

#define PCB_FUNC_NOT_EXECUTED   0
#define PCB_FUNC_EXECUTING      1
#define PCB_FUNC_FINISHED      -1

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

/* Premier élément de la liste de PCB */
extern struct pcb_s * _first_pcb;

/* Dernier élément de la liste de PCB */
extern struct pcb_s * _last_pcb;

/* Pointeur vers le PCB courant */
extern struct pcb_s * _current_pcb;


/* \brief Crée un processus à partir d'une function qui prend un paramètre
 * \param entry function d'entrée du processus
 * \param args  argument à donner à la fonction à l'appel
 *
 * Crée un processus en assignant un bloc de control de processus (PCB).
 * Ce bloc contient l'adresse du code du processus à exécuter ainsi que le
 * paramètre à lui transmettre à l'utilisation. Il possède également
 * l'historique du contexte permettant de réentrer dans la fonction.
 * Le contexte est composé du Program Counter (PC), de la Stack Pointer (SP)
 * et de la table des registres (R0 à R12) utilisé.
 * Ajoute ce processus à la liste des processus à ordonnancer. La fonction
 * yield permettra de changer de contexte avec le suivant sur la liste.
 * Lorsque la fonction d'un processus est fini, le processus est libéré puis
 * retiré de la liste des processus à ordonnancer.
 *
 * Si la fonction passé en paramètre est un pointeur de fonction nul c.-à-d.
 * que sa valeur est 0, alors la création du processus est ignorée.
 * Si l'argument est un pointeur nul, il est tout de meme considéré comme
 * valide.
 */
extern void create_process(func_t entry, void * args);

#endif
