#ifndef SCHED_H
#define SCHED_H

#include "process.h"

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

/* \brief Décide du pcb à exécuter, en "nettoyant" les pcb terminés
 *
 * TODO commentaire détaillé
 */
extern struct pcb_s * schedule();

#endif
