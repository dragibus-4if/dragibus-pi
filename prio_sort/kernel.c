#include "../os/hw.h"
#include "../os/sched.h"
#include "../os/malloc.h"
#include "../os/sem.h"

int sorted[10];
int cursor = 0;

void proc(void * args) {
    int v = (int) args;
    sorted[cursor] = v;
    cursor++;
}

//------------------------------------------------------------------------
int start_kernel(void) {
    init_hw();
    malloc_init((void *) HEAP_START);
    int unsorted[10] = {4, 3, 5, 0, 9, 7, 1, 6, 2, 8};
    for (int i = 0 ; i < 10 ; i++) {
        create_process(&proc, (void *) unsorted[i], 128, SCHED_OTHER, unsorted[i]);
    }
    sched_start();
    return 0;
}
