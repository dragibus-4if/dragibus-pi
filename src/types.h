/* types.h */
#ifndef TYPES_H
#define TYPES_H

#ifndef _GCC_STDINT_H
#define _GCC_STDINT_H

#ifndef NULL
#  define NULL 0
#endif

/* Définie des types entiers à taille fixe. */
typedef char                int8_t;
typedef int                 int16_t;
typedef long int            int32_t;

typedef unsigned char       uint8_t;
typedef unsigned int        uint16_t;
typedef unsigned long int   uint32_t;

typedef unsigned long int   size_t;

/* Définie un type de pointeur de fonction */
typedef void (*func_t)(void);

#endif

#endif