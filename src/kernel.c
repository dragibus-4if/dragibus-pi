#include "hw.h"
#include "sched.h"
#include "malloc.h"
#include "sem.h"

struct sem_s sema;

void processus_A(void * args) {
    int i = 0;
    while (1) {
        sem_down(&sema);
        led_on();
        sem_up(&sema);
				i=0;							//Mis ici pour une comodité de breakpoint (pas d'influence)
        while (i++ < 2000000);
        i = 0;
    }
}

void processus_B(void * args) {
    int i = 0;
    while (1) {
        sem_down(&sema);
        while (i++ < 1000000);
        led_off();
        sem_up(&sema);
				i = 0;					//Mis après pour une comodité de breakpoint
        while (i++ < 1000000);
        i = 0;
    }
}


//------------------------------------------------------------------------
int start_kernel(void) {
    init_hw();
    malloc_init((void *) HEAP_START);
    struct sem_s sema;
    sem_init(&sema, 1);
    create_process(&processus_A, (void *) 0, 512);
    create_process(&processus_B, (void *) 0, 512);
    start_sched();
    return 0;
}
