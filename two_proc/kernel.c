#include "hw.h"
#include "sched.h"
#include "malloc.h"
#include "sem.h"

void processus_A(void * args) {
    struct sem_s sema = *(struct sem_s *) args;
    int i = 0;
    while (1) {
        sem_down(&sema);
        led_on();
        sem_up(&sema);
        while (i++ < 2000000);
        i = 0;
    }
}

void processus_B(void * args) {
    struct sem_s sema = *(struct sem_s *) args;
    int i = 0;
    while (1) {
        sem_down(&sema);
        led_off();
        sem_up(&sema);
        while (i++ < 2000000);
        i = 0;
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
