#include "../os/hw.h"
#include "../os/sched.h"
#include "../os/malloc.h"
#include "../os/sem.h"
#include "../os/mutex.h"

#define SLEEP_TIME_A 1000000
#define SLEEP_TIME_B 3000000

void processus_A(void * args) {
    struct mutex_s * mtxa = (struct mutex_s *) args;
    while (1) {
        mutex_acquire(mtxa);
        for (int j = 0 ; j < 10 ; j++) {
            sched_forced_yield();
            led_on();
            for(int i = 0 ; i < SLEEP_TIME_A ; i++);
            sched_forced_yield();
            led_off();
            for(int i = 0 ; i < SLEEP_TIME_A ; i++);
        }
        mutex_release(mtxa);
        sched_forced_yield();
    }
}

void processus_B(void * args) {
    struct mutex_s * mtxa = (struct mutex_s *) args;
    while (1) {
        mutex_acquire(mtxa);
        for (int j = 0 ; j < 10 ; j++) {
            sched_forced_yield();
            led_on();
            for(int i = 0 ; i < SLEEP_TIME_B ; i++);
            sched_forced_yield();
            led_off();
            for(int i = 0 ; i < SLEEP_TIME_B ; i++);
        }
        mutex_release(mtxa);
        sched_forced_yield();
    }
}


//------------------------------------------------------------------------
int start_kernel(void) {
    init_hw();
    malloc_init((void *) HEAP_START);
    struct mutex_s  * mutexa = mutex_create();
    create_process(&processus_A, (void *) mutexa, 512, SCHED_OTHER, 20);
    create_process(&processus_B, (void *) mutexa, 512, SCHED_OTHER, 20);
    sched_start();
    return 0;
}
