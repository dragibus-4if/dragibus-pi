#ifndef SCHED_H
#define SCHED_H

#include "types.h"

typedef void (func_t)(void);

enum pcb_state_e { READY, NEW, TERMINATED, WAITING };

struct pcb_s {
  /* Pointeur de pile */
  uint32_t * sp;

  /* Fonction et arguments */
  func_t * entry_point;
  void * args;

  size_t size;
  char * stack_base;
  enum pcb_state_e state;

  struct pcb_s * next;
};

/* Gestion de l'ordonnancement */
enum sched_mode_e { BASIC, PRIORITY };
void yield();
void schedule();
void start_sched();
void set_sched_mode(enum sched_mode_e mode);

/* Gestion d'un processus */
int create_process(func_t * f, size_t size);
void process_block();
void process_release(struct pcb_s* pcb);

/* Gestion du process courant */
void start_current_process();
struct pcb_s * get_current_process();

#endif
