#ifndef SEM_H
#define SEM_H

#include "types.h"

struct sem_s;

void sem_init(struct sem_s * sem, int val);
void sem_up(struct sem_s * sem);
void sem_down(struct sem_s * sem);

#endif
