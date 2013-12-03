#ifndef PIPE_H
#define PIPE_H

#include "types.h"

/**
 * Création d'un pipe, modifiant les descripteurs de pipe d'entrée *in_des* et
 * de sortie *out_des*. Retourne 0 si le pipe a bien été crée, -1 sinon.
 */
extern int pipe_create(intptr_t * in_des, intptr_t * out_des);

/**
 * Ferme le pipe *des*.
 */
extern int pipe_close(intptr_t des);

/**
 * Lit au maximum *bufsize* octets depuis le descripteur de pipe *des* dans
 * *buffer*, un buffer de sortie d'une taille d'au moins *bufsize*.
 * Retourne le nombre d'octets lus, ou -1 si une erreur a eu lieu.
 */
extern ssize_t pipe_read(intptr_t des, void * buffer, size_t bufsize);

/**
 * Écrit au maximum *bufsize* octets à partir du descripteur de pipe *des*
 * dans *buffer*, un buffer d'entrée d'une taille d'au moins *bufsize*.
 * Retourne le nombre d'octets écrits, ou -1 si une erreur a eu lieu.
 */
extern ssize_t pipe_write(intptr_t des, const void * buffer, size_t bufsize);

#endif
