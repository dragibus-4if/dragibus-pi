#include <stdint.h>
#include <malloc.h>

#include "hw.h"
#include "registers.h"
#include "uart.h"
#include "audio.h"
#include "music.h"
#include <sys/types.h>

#define INTERVAL 0x00020000

extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );

extern unsigned int bss_start;
extern unsigned int bss_end;

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
  unsigned int rx = GET32(SYSTIMERCLO);
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
hw_init()
{
    unsigned int ra;
    unsigned int rx;

    for(unsigned int i=bss_start;i<bss_end;i+=4)
      PUT32(i,0);
    uart_init();
    
    /* Init audio interface */
    /* music_init(); */

    /* /\* Make gpio pin tied to the led an output *\/ */
    /* ra=GET32(GPFSEL1); */
    /* ra&=~(7<<18); */
    /* ra|=1<<18; */
    /* PUT32(GPFSEL1,ra); */
    /* PUT32(GPSET0,1<<16); //led off */

    
    /* Set up delay before timer interrupt (we use CM1) */
    rx=GET32(SYSTIMERCLO);
    rx += INTERVAL;
    PUT32(C1,rx);
    
    /* Enable irq triggering by the *system timer* peripheral */
    /*   - we use the compare module CM1 */
    enable_timer_irq();
    
    /* Enable interrupt *line* */
    PUT32(0x2000B210, 0x00000002);
}

static uint32_t timer_ms_base; // 'zero' time

uint32_t
get_timer_ms()
{
    // Return ms elapsed since timer_ms_base. This is currently approximate - should do the math properly.
    return ((((uint64_t)GET32(SYSTIMERCHI) << 32) | (uint64_t)GET32(SYSTIMERCLO)) >> 10) - timer_ms_base;
}


uint32_t
reset_timer_ms()
{
    timer_ms_base = 0;
    timer_ms_base = get_timer_ms();
    return timer_ms_base;
}

extern int dummy(int);

int usleep(useconds_t __useconds) {
    // Pause for about t ms
    int i;
    unsigned int t = __useconds;
    for (;t>0;t--) {
        for (i=5000;i>0;i--)
	  dummy(i);
    }
    return __useconds;
}

