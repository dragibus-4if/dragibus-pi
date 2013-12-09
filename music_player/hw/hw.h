#ifndef HW_H
#define HW_H

#include <stdint.h>
#include <sys/types.h>

#define ENABLE_IRQ()				\
  __asm("\tpush {r0}\n\t"				\
      "mrs r0,cpsr\n\t"				\
      "bic r0,r0,#0x80\n\t"			\
      "msr cpsr_c,r0\n\t"			\
      "pop {r0}"				\
      :						\
      :						\
      : "r0");

#define DISABLE_IRQ()				\
  __asm("\tpush {r0}\n\t"				\
      "mrs r0,cpsr\n\t"				\
      "orr r0,r0,#0x80\n\t"			\
      "msr cpsr_c,r0\n\t"			\
      "pop {r0}"				\
      :						\
      :						\
      : "r0");

void enable_timer_irq();
void disable_timer_irq();
void set_next_tick_and_enable_timer_irq();

void led_off();
void led_on();

uint32_t get_timer_ms();
uint32_t reset_timer_ms();
int usleep(useconds_t __useconds);

void hw_init();

#endif
