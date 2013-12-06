#ifndef SCHED_H
#define SCHED_H

#include "types.h"

extern struct pcb_s* current_process;

typedef void (func_t)(void *);

enum pcb_state_e {READY, NEW, TERMINATED, WAITING};

struct pcb_s {
  /* Pointer to stack */
  uint32_t* sp;

  /* function and args */
  func_t* entry_point;
  void* args;

  size_t size;
  char* stack_base;
  enum pcb_state_e state;

  struct pcb_s *next;
};


int create_process(func_t* f, void * args, size_t size);
void yield();
void start_sched();
void schedule();
void start_current_process();
void process_block();
void process_release(struct pcb_s* pcb);
struct pcb_s * get_current_process();

#endif
