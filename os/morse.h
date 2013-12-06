#ifndef MORSE_H
#define MORSE_H

#include "types.h"

/* Constante définissant le temps d'une pause, à jauger selon les observations
 * sur le programme */
#define MORSE_PAUSE_TIME 1000

/* Constante définissant la vitesse d'un court par rapport à un long */
#define MORSE_LONG_SHORT_RATIO 3

/* Blink the led corresponding to the character given */
void morse_blink_char(char c);

/* Blink for each character in the char array */
void morse_blink_buffer(const char * buffer, size_t size);

#endif
