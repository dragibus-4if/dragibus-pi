#include "process.h"
#include "dispatcher.h"

/* TODO Place ça dans un autre fichier et choisir une valeur non aléatoire
*/
#define STACK_SIZE 128

struct ctx_s ctx_A;
struct ctx_s ctx_B;

void funcA()
{
    int cptA = 0;
    while(1) {
        cptA++;
        switch_to(&ctx_B);
    }
    /* switch_to(&ctx_B); */
    /* switch_to(&ctx_B); */
    /* switch_to(&ctx_B); */
    /* switch_to(&ctx_B); */
    /* switch_to(&ctx_B); */
    /* switch_to(&ctx_B); */
}

void funcB()
{
    int cptB = 1;
    while(1) {
        cptB += 2;
        switch_to(&ctx_A);
    }
    /* switch_to(&ctx_A); */
    /* switch_to(&ctx_A); */
    /* switch_to(&ctx_A); */
    /* switch_to(&ctx_A); */
    /* switch_to(&ctx_A); */
    /* switch_to(&ctx_A); */
}

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
    /* init_ctx(&ctx_B, funcB, STACK_SIZE); */
    /* start_ctx(&ctx_A, funcA); */

    create_process(pcbFuncA, NULL);
    yield();

    /* Pas atteignable vues nos 2 fonctions */
    return 0;
}
