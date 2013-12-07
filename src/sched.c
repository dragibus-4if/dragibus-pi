#include "sched.h"
#include "malloc.h"
#include "hw.h"
#include "dispatcher.h"

struct pcb_s idle;
struct pcb_s* ready_queue = (struct pcb_s *) 0;
struct pcb_s* current_process = (struct pcb_s *) NULL;
struct pcb_s* waiting_queue = (struct pcb_s *) 0;

#define PRINT(MSG) ;
#define EXIT(CODE) ;

struct pcb_s * get_current_process() {
  return current_process;
}

void process_block(){
  struct pcb_s* it = current_process;

	//On cherche le process précédent
  while (it->next != current_process) {
    it=it->next;
  }
	//On enlève le process courant de la ready_queue
  it->next = it->next->next;

  it= waiting_queue;
	//On teste si la waiting queue est initialisée
  if (waiting_queue == 0 )
    {
        waiting_queue->next = current_process;
    } else {
				//Si elle l'est on cherche le dernier élément 
        while(it->next != waiting_queue){
        it=it->next;
        }
    }

	//On met le process courant dans la waiting queue (en fin)
  it->next = current_process;    
  current_process->next = waiting_queue;
  current_process->state = WAITING;
}

void process_release(struct pcb_s* pcb){
  struct pcb_s* it = waiting_queue;
	//Recherche de l'elt de la waiting qui pointe sur le pcb voulu  
	while(it->next != pcb){
    it=it->next;
  }

	//On retire pcb de la waiting
  it->next = it->next->next;

	//On remet en l'état pour execution normale
  pcb->next = current_process->next;
  current_process->next = pcb;
  pcb->state = READY;
}


/**
 *	Il faudrait implem cette fonction
 *  	Pour le moment elle fait un fiq (je ne sais même pas pourquoi !)
**/
void process_test(int val){
	while (current_process->state != READY || val >= 0){
		;	
	};
}

void start_current_process()
{
  current_process->state = READY;
  current_process->entry_point(current_process->args);

  /* The process is terminated */
  current_process->state = TERMINATED;
  yield();
}

int init_process(struct pcb_s *pcb, size_t stack_size, func_t * f, void * args)
{	
  /* Function and args */
  pcb->entry_point = f;
  pcb->args = args;

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

int create_process(func_t* f, void * args, size_t size)
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
  return init_process(pcb, size, f, args);
}


void schedule()
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

void yield()
{
  ctx_switch();
}

void start_sched()
{
  current_process = &idle;
  idle.next = ready_queue;
  
   

  ENABLE_IRQ();

  while(1) {
    yield();
  }
}
