#include "../os/malloc.h"
#include "../os/hw.h"
#include "../os/sched.h"
#include "../os/sem.h"

#define LITTLE_SLEEP_TIME 500
#define SLEEP_TIME 500000

void fork_bomb(void * args) {
    size_t * nbr = (size_t *)((void **)args)[0];
    struct sem_s * sem = (struct sem_s *)((void **)args)[1];
    (*nbr)++;
    while(1) {
        sem_down(sem);
        size_t n = *nbr;
        led_on();
        for(size_t i = 0 ; i < n * LITTLE_SLEEP_TIME ; i++);
        led_off();
        for(size_t i = 0 ; i < SLEEP_TIME ; i++);
        sem_up(sem);
        create_process(&fork_bomb, args, 128);
    }
}

int start_kernel(void) {
    malloc_init((void *) HEAP_START);
    init_hw();
    struct sem_s sem;
    sem_init(&sem, 1);
    size_t n = 0;
    void * args[2];
    args[0] = (void *) &n;
    args[1] = (void *) &sem;
    create_process(&fork_bomb, args, 128);
    sched_start();
    return 0;
}

/* vim: set ft=c et sw=4 sts=4 */

