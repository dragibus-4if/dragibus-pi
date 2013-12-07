#include "../os/hw.h"
#include "../os/sched.h"
#include "../os/malloc.h"
#include "../os/sem.h"

#define SLEEP_TIME_A 1000000
#define SLEEP_TIME_B 3000000

void processus_A(void * args) {
    struct sem_s * sema = (struct sem_s *) args;
    while (1) {
        sem_down(sema);
        for (int j = 0 ; j < 10 ; j++) {
            yield();
            led_on();
            for(int i = 0 ; i < SLEEP_TIME_A ; i++);
            yield();
            led_off();
            for(int i = 0 ; i < SLEEP_TIME_A ; i++);
        }
        sem_up(sema);
        yield();
    }
}

void processus_B(void * args) {
    struct sem_s * sema = (struct sem_s *) args;
    while (1) {
        sem_down(sema);
        for (int j = 0 ; j < 10 ; j++) {
            yield();
            led_on();
            for(int i = 0 ; i < SLEEP_TIME_B ; i++);
            yield();
            led_off();
            for(int i = 0 ; i < SLEEP_TIME_B ; i++);
        }
        sem_up(sema);
        yield();
    }
}


//------------------------------------------------------------------------
int start_kernel(void) {
    init_hw();
    malloc_init((void *) HEAP_START);
    struct sem_s sema;
    sem_init(&sema, 1);
    create_process(&processus_A, (void *) &sema, 512);
    create_process(&processus_B, (void *) &sema, 512);
    start_sched();
    return 0;
}
