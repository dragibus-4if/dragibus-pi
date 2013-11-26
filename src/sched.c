#include "sched.h"
#include "malloc.h"
#include "hw.h"
#include "dispatcher.h"

struct pcb_s idle;
struct pcb_s* ready_queue = (struct pcb_s *)0;
struct pcb_s* current_process = (struct pcb_s *) NULL;

extern void processus_A();

#define PRINT(MSG) ;
#define EXIT(CODE) ;

void
start_current_process()
{
  current_process->state = READY;
  current_process->entry_point();

  /* The process is terminated */
  current_process->state = TERMINATED;
  yield();
}

int
init_process(struct pcb_s *pcb, int stack_size, func_t* f)
{	
  /* Function and args */
  pcb->entry_point = f;

  /* Stack allocation */
  pcb->size=stack_size;
  pcb->stack_base = malloc_alloc(stack_size);
  if(!pcb->stack_base)
    return 0;

  /* State and context */
  pcb->state = NEW;
  pcb->sp = ((uint32_t*) (pcb->stack_base + stack_size)) - 1;

  /* Fill in the stack with CPSR and PC */
  *(pcb->sp) = 0x53;
  pcb->sp --;
  *(pcb->sp) = (unsigned int) &start_current_process;
  
  return 1;
}

int
create_process(func_t* f, unsigned size)
{
  struct pcb_s *pcb;
  pcb = (struct pcb_s*) malloc_alloc(sizeof(struct pcb_s));

  if(!pcb)
    return 0;

  if (! ready_queue) {/* First process */
    ready_queue = pcb;
  } else {
    pcb->next = ready_queue->next;
  }
  
  ready_queue->next = pcb;
  return init_process(pcb,size,f);
}


void
schedule()
{
  struct pcb_s* pcb;
  struct pcb_s* pcb_init;

  pcb_init = current_process;
  pcb = current_process;

  /* Start by eliminating all zombies (rhaaaaa !) */
  while (pcb->next->state == TERMINATED) {    
    /* If no process to be executed -> note that and leave loop */
    if (pcb->next == pcb) {
      pcb = NULL;
      break;

    } else {
      /* Particular case of the head */
      if (pcb->next == ready_queue)
	ready_queue = pcb;    
      
      /* Remove pcb from the list (FIXME : could be done after the loop) */
      pcb->next = pcb->next->next;

      /* Free memory */
      malloc_free((char*) pcb->next->stack_base);
      malloc_free((char*) pcb->next);

      /* Get next process */
      pcb = pcb->next;
    }
  }

  if (pcb != NULL) {
    /* On parcours la liste jusqu'à trouver un processus non bloqué */
    pcb = pcb->next;    
    while(pcb->state == WAITING && pcb != pcb_init)
      pcb = pcb->next;

    /* Si tous les processus sont bloqués -> on le note */
    if(pcb->state == WAITING)
      pcb = NULL;
  }

  if(pcb == NULL) {   /* Si pas de processus à élire -> idle */
    ready_queue = NULL;
    current_process = &idle;
  } else {            /* Sinon -> le processus élu est le suivant */
      current_process = pcb;
  }
}

void
yield()
{
  ctx_switch();
}

void
start_sched()
{
  current_process = &idle;
  idle.next = ready_queue;

  ENABLE_IRQ();

  while(1) {
    yield();
  }
}
