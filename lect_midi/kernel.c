#include "../os/hw.h"
#include "../os/sched.h"
#include "../os/malloc.h"
#include "../os/sem.h"
#include "../os/mutex.h"


void processus_A(void * args) {

    }
}



//------------------------------------------------------------------------
int start_kernel(void) {
    init_hw();
    malloc_init((void *) HEAP_START);
    create_process(&processus_A, (void *) 0, 512);
    start_sched();
    return 0;
}
