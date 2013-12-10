#include "hw.h"
#include "types.h"
#include "registers.h"
#include "uart.h"
#include "audio.h"
#include "../lect_midi/music/music.h"

#define CS      0x20003000
#define CLO     0x20003004
#define C0      0x2000300C
#define C1      0x20003010
#define C2      0x20003014
#define C3      0x20003018


#define GPFSEL1 0x20200004
#define GPSET0  0x2020001C
#define GPCLR0  0x20200028

#define INTERVAL 0x00080000

extern void PUT32(unsigned int, unsigned int);
extern unsigned int GET32(unsigned int);
extern unsigned int bss_start;
extern unsigned int bss_end;

/*
 * Timer interrupts
 */
#define ENABLE_TIMER_IRQ() PUT32(CS, 2)
#define DISABLE_TIMER_IRQ() PUT32(CS, ~2);

void set_tick_and_enable_timer() {
  unsigned int rx = GET32(CLO);
  rx += INTERVAL;
  PUT32(C1, rx);
  ENABLE_TIMER_IRQ();
}


/* LEDs on/off */

void led_on() {
  PUT32(GPCLR0, 1 << 16);
}

void led_off() {
  PUT32(GPSET0, 1 << 16);
}

/* Start_hw */
void init_hw() {
    unsigned int ra;
    unsigned int rx;

    /* Make gpio pin tied to the led an output */
    ra = GET32(GPFSEL1);
    ra &= ~(7 << 18);
    ra |= 1 << 18;
    PUT32(GPFSEL1, ra);

    /* led off */
    PUT32(GPSET0, 1 << 16);

    for(unsigned int i=bss_start;i<bss_end;i+=4)
    PUT32(i,0);
    uart_init();

    /* Set up delay before timer interrupt (we use CM1) */
    rx=GET32(CLO);
    rx += INTERVAL;
    PUT32(C1, rx);

    /* Enable irq triggering by the *system timer* peripheral
     * - we use the compare module CM1 */
    ENABLE_TIMER_IRQ();

    /* Enable interrupt *line* */
    PUT32(0x2000B210, 0x00000002);
    DISABLE_IRQ();
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
