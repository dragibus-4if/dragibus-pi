/* types.h */
#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

/* Définie un type de pointeur de fonction */
typedef void (*func_t)();

#ifndef NULL
#  define NULL 0
#endif

typedef uint32_t size_t;

#endif
