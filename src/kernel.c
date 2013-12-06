#include "hw.h"
#include "sched.h"
#include "malloc.h"
#include "sem.h"

struct sem_s sema;


void
processus_A()
{
  int i = 0;
	  
  while (1) {
		sem_down(&sema);    
		led_on();
		sem_up(&sema);
    while ( i++ < 2000000);
		
    i = 0;
  }
}

void
processus_B()
{
  int i = 0;

  while (1) {
    sem_down(&sema);    
		led_off();
		
    while ( i++ < 2000000);
		sem_up(&sema);
    i = 0;
  }
}


//------------------------------------------------------------------------
int
start_kernel ( void )
{
  DISABLE_IRQ();
  init_hw();
  malloc_init((void *) HEAP_START);

	sem_init(&sema,5);

  create_process(&processus_A, 512);
  create_process(&processus_B, 512);
  
  start_sched();
  
  return 0;
}
