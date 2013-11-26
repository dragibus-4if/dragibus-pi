#include "sched.h"
#include "process.h"

static void pcbFuncA(void * args) {
    int cptA = 1;
    while (1) {
        cptA++;
        if (cptA % 1337 == 0) {
            cptA = 0;
            yield();
        }
    }
}

static void pcbFuncB(void * args) {
    int cptB = 1;
    while (1) {
        cptB++;
        if (cptB % 1337 == 0) {
            cptB = 0;
            yield();
        }
    }
}

static void pcbFuncC(void * args) {
    int i;
    for (i = 42; i > 0; i--);
    yield();
    for (i = 42; i > 0; i--);
}

int notmain(void)
{
    /* Ajout des processus */
    create_process(pcbFuncA, NULL);
    create_process(pcbFuncB, NULL);
    create_process(pcbFuncC, NULL);

    /* Démarrage de l'ordonnanceur */
    yield();

    return 0;
}
