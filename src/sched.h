#ifndef SCHED_H
#define SCHED_H

#include "process.h"

/* TODO commentaires */

pcb_s* chainePCB;

void create_process(func_t f, void* args);

void yield();

void start_current_process();

#endif
