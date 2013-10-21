#include "process.h"
#include "dispatcher.h"

void pcbFuncA(void * args) {
    int cptA = 1;
    while(1) {
        cptA++;
        if(cptA % 1337 == 0) {
            cptA = 0;
            yield();
        }
    }
}

//------------------------------------------------------------------------
int notmain(void)
{
     create_process(pcbFuncA, NULL);
     yield();

    return 0;
}
