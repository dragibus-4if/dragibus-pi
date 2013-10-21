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
    int32_t regs[13];
};

struct ctx_s * current_ctx;

extern void init_ctx(struct ctx_s * ctx, func_t f, size_t stack_size);

extern void start_ctx(struct ctx_s * ctx, func_t f);

/* Espace utilisateur */

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

/* \brief Sauvegarde le contexte courant et passe la main au suivant
 *
 * Enregistre le contexte du processus en cours d'utilisation puis
 * sélectionne le processus suivant à exécuter. Le contexte de ce dernier est
 * chargé en mémoire puis est défini comme le courant.
 *
 * Si il n'y a pas de processus en exécution, cette fonction s'exécute tout de
 * meme sans enregistrer précedemment le contexte.
 * Si il n'y a pas de processus à exécuter, cette fonction ne fait rien.
 */
extern void yield();

#endif
