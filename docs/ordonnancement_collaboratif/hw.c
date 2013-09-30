#include "hw.h"

#define CS 0x20003000
#define CLO 0x20003004
#define C0 0x2000300C
#define C1 0x20003010
#define C2 0x20003014
#define C3 0x20003018

#define GPFSEL1 0x20200004
#define GPSET0  0x2020001C
#define GPCLR0  0x20200028

#define INTERVAL 0x00080000

extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );

/*
 * Timer interrupts
 */
void
enable_timer_irq()
{
  /* Enable timer interrupt */
  PUT32(CS,2);
}

void
disable_timer_irq()
{
  /* Disable timer interrupt */
  PUT32(CS,~2);
}


void
set_next_tick_and_enable_timer_irq()
{
  unsigned int rx = GET32(CLO);
  rx += INTERVAL;
  PUT32(C1,rx);

  enable_timer_irq();
}


/*
 * LEDs on/off
 */

void
led_off()
{
  PUT32(GPSET0,1<<16); //led off
}

void
led_on()
{
  PUT32(GPCLR0,1<<16); //led on
}

/*
 * Start_hw
 */
void
init_hw()
{
    unsigned int ra;
    unsigned int rx;

    /* Make gpio pin tied to the led an output */
    ra=GET32(GPFSEL1);
    ra&=~(7<<18);
    ra|=1<<18;
    PUT32(GPFSEL1,ra);
    PUT32(GPSET0,1<<16); //led off
    
    /* Set up delay before timer interrupt (we use CM1) */
    rx=GET32(CLO);
    rx += INTERVAL;
    PUT32(C1,rx);
    
    /* Enable irq triggering by the *system timer* peripheral */
    /*   - we use the compare module CM1 */
    enable_timer_irq();
    
    /* Enable interrupt *line* */
    PUT32(0x2000B210, 0x00000002);
}
