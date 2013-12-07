#include "../os/malloc.h"
#include "../os/hw.h"
#include "../os/sched.h"

#define SLEEP_TIME 1000000

void turn_on(void * a) {
    while(1) {
        led_on();
        for(int i = 0 ; i < SLEEP_TIME ; i++);
        yield();
    }
}

void turn_off(void * a) {
    while(1) {
        led_off();
        for(int i = 0 ; i < SLEEP_TIME ; i++);
        yield();
    }
}

int start_kernel(void) {
    malloc_init((void *) HEAP_START);
    init_hw();
    create_process(&turn_on, (void *) NULL, 128);
    create_process(&turn_off, (void *) NULL, 128);
    start_sched();
    return 0;
}

/* vim: set ft=c et sw=4 sts=4 */
