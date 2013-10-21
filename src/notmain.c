#include "sched.h"
#include "process.h"

static void pcbFuncA(void * args) {
    int cptA = 1;
    while(1) {
        cptA++;
        if(cptA % 1337 == 0) {
            cptA = 0;
            yield();
        }
    }
}

static void pcbFuncB(void * args) {
    int cptB = 1;
    while(1) {
        cptB++;
        if(cptB % 1337 == 0) {
            cptB = 0;
            yield();
        }
    }
}

//------------------------------------------------------------------------
int notmain(void)
{
     create_process(pcbFuncA, NULL);
     create_process(pcbFuncB, NULL);
     yield();

    return 0;
}
