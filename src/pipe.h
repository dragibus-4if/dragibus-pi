#ifndef PIPE_H
#define PIPE_H

#include "types.h"

typedef int pipe_t;

/**
 * La fonction *pipe_create* créé un pipe anonyme et place les descripteurs de
 * pipe pour la lecture et l'écriture (respectivement) dans *in_des* et
 * *out_des*.
 * En cas de succès, la fonction retourne 0. Sinon, -1 est retourné. Les échecs
 * peuvent etre entraînés par les raisons suivantes :
 * - Les pointeurs de descripteurs sont invalides.
 * - Il n'y a plus assez de mémoire dans le système.
 */
extern int pipe_create(pipe_t * in_des, pipe_t * out_des);

/**
 * La fonction *pipe_close* ferme l'extrémité de pipe décrit par *des*. Lorsque
 * les deux extrémités d'un meme pipe sont fermé, les données écrites, meme si
 * elles ne sont pas lues, sont détruites et désallouées.
 * En cas de succès, la fonction retourne 0. Sinon, -1 est retourné. Les échecs
 * peuvent etre entraînés par les raisons suivantes :
 * - Le descripteur de pipe *des* n'est pas valide.
 * TODO gérer le cas où la fonction a été interrompu par un signal ?
 * TODO que faire lorsque quelqu'un lit/écrit en meme temps dans le pipe ?
 */
extern int pipe_close(pipe_t des);

/**
 * La fonction *pipe_read* lit jusqu'à *bufsize* bytes depuis le pipe avec le
 * descripteur *des*, et enregistre le résultats dans *buffer* (ce n'est pas
 * forcément une chaîne de caractère et aucun caractère de fin n'est ajouté).
 * La valeur de retour correspond au nombre de bytes réellement lues. Cela peut
 * etre moins que *bufsize*; par exemple si il n'y a pas assez de bytes restant
 * à lire dans le pipe. Il est à noter que lire moins de ùbufsize* bytes n'est
 * pas une erreur.
 * Si *pipe_read* retourne au moins un caractère, il n'est pas possible de
 * savoir si la fin du pipe a été atteint. Mais si on atteint la fin, les
 * prochains appels de *pipe_read* retournerons 0.
 * En cas d'erreur, *pipe_read* retourne -1. Les échecs peuvent etre entraînés
 * par les raisons suivantes :
 * - Le descripteur de pipe *des* n'est pas valide.
 * - Le pointeur de *buffer* n'est pas valide.
 * - Le pipe n'est ouvert qu'en écriture et pas en lecture
 * TODO gérer le cas où la fonction a été interrompu par un signal ?
 */
extern ssize_t pipe_read(pipe_t des, void * buffer, size_t bufsize);

/**
 * La fonction *pipe_write* écrit jusqu'à *bufsize* bytes de *buffer* dans le
 * pipe dont le descripteur est *des*. *buffer* ne contient pas forcément une
 * chaîne de caractère et un caractère nul est écrit comme n'importe quel autre
 * caractère.
 * La valeur de retour est le nombre de bytes réellement écrits. Cette valeur
 * peut etre inférieur à *bufsize*. Les programmes devraient toujours appeler
 * *pipe_write* dans une boucle, s'exécutant tant que toutes les données ne
 * sont pas écrites.
 * En cas d'erreur, *pipe_write* retourne -1. Les échecs peuvent etre entraînés
 * par les raisons suivantes :
 * - Le descripteur de pipe *des* n'est pas valide.
 * - Le pointeur de *buffer* n'est pas valide.
 * - Le pipe n'est ouvert qu'en lecture et pas en écriture
 * TODO taille du pipe trop grand ?
 * TODO gérer le cas où la fonction a été interrompu par un signal ?
 */
extern ssize_t pipe_write(pipe_t des, const void * buffer, size_t bufsize);

#endif
