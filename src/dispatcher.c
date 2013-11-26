#include "hw.h"
#include "sched.h"

void __attribute__ ((naked))
ctx_switch ( void )
{
  /********* Save context of running process ********/
  /*   Save PC and status register of interrupted task on SVC stack */	
  __asm volatile("sub      lr, lr, #4");
  __asm volatile("srsdb    sp!, 0x13");
  
  /*   Switch to SVC mode */	        
  __asm volatile("cps	#0x13");
  
  /*   Save registers on SVC stack */					
  __asm volatile("push    {r0-r12}");

  /* Disable interrupts */
  DISABLE_IRQ();

  /*   Save actual stack pointer */
  __asm("mov %0, sp"
  	: "=r"(current_process->sp));

  /********* Choose next task ********/
  schedule();
  
  /******* Restore context of elected process ********/
  /*   Get stack pointer of the next process... */
  __asm("mov sp, %0"
  	:
  	: "r"(current_process->sp));
  
  /* Set up delay before new interrupt and enable interrupts */
  set_tick_and_enable_timer();
  
  if (current_process->state == NEW) {
    
    /* Just to avoid that wrong information appears when using 'bt' in gdb */
    __asm("mov lr, #0x0");

  } else {
    /*   Restore non banked registers */
    __asm volatile("pop     {r0-r12}");
  }

  /* Jump to elected task, popping cpsr and pc from SVC stack  */
  /* No need to explicitly enable IRQs as 'rfe' restore cpsr */
  __asm volatile ("rfefd sp!");
}
