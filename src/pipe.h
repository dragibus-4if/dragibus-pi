#ifndef PIPE_H
#define PIPE_H

#include "types.h"

/**
 * Création d'un pipe, modifiant les descripteurs de pipe d'entrée *in_des* et
 * de sortie *out_des*. Retourne 0 si le pipe a bien été crée, -1 sinon.
 */
int pipe_create(int * in_des, int * out_des);

/**
 * Ferme le pipe *des*.
 */
int pipe_close(int des);

/**
 * Lit au maximum *bufsize* octets depuis le descripteur de pipe *des* dans
 * *buffer*, un buffer de sortie d'une taille d'au moins *bufsize*.
 * Retourne le nombre d'octets lus, ou -1 si une erreur a eu lieu.
 */
ssize_t pipe_read(int * des, void * buffer, size_t bufsize);

/**
 * Écrit au maximum *bufsize* octets à partir du descripteur de pipe *des*
 * dans *buffer*, un buffer d'entrée d'une taille d'au moins *bufsize*.
 * Retourne le nombre d'octets écrits, ou -1 si une erreur a eu lieu.
 */
ssize_t pipe_write(int * des, const void * buffer, size_t bufsize);

#endif
