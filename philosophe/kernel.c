#include "../os/malloc.h"
#include "../os/hw.h"
#include "../os/sched.h"
#include "../os/sem.h"

#define LITTLE_SLEEP_TIME 500
#define SLEEP_TIME 500000
#define NBPHILO = 5;

typedef enum {PENSE, AFAIM, MANGE} ETAT_PHILO;
ETAT_PHILO etat_philo[NBPHILO];
sem_s sem_philo[NBPHILO];
sem_s sem_tab;
int ids[NBPHILO];

void start_philo() {
    sem_init(&sem_tab,1);
    for(int i =0; i< NBPHILO;i++){           
        ids[i]=i;  
        etat_philo[i] = PENSE;
        sem_init(&sem_philo[i],1);
    }
    for(int i =0;i<NBPHILO;i++){
         void * args[1];
         args[0] = (void *) &ids[i];
         create_process(&doPhilosophe,args, 
   }
}

void doPhilosophe(int id){
    sem_down(&sem_tab);
    ETAT_PHILO temp=etat_philo[id];
    sem_up(&sem_tab);
    if(temp==MANGE){
       sem_down(&sem_philo[id]);
       int i=0;
       while(i++<20000000);
       sem_up(&sem_philo[id]);
       sem_down(&sem_tab);
       etat_philo[id]=PENSE; 
       sem_up(&sem_tab);        
    }else if(temp==PENSE){
        int i =0;
        while(i++<2000000);
        sem_down(&sem_tab);
        etat_philo[id]=AFAIM;
        sem_up(&sem_tab);
    }else if(temp==AFAIM){
        sem_down(&sem_tab);
        if(etat_philo[(id-1)%NBPHILO]!=MANGE && etat_philo[(id+1)%NBPHILO]!= MANGE){
            etat_philo[id]=MANGE;
        }
        sem_up(&sem_tab);
        int i =0;
        while(i++<3000000);
    }
}
/*
void test_mange(int i){
    if(etat_philo[i] == A_FAIM && etat_philo[(i+1)%NBPHILO] != MANGE && etat_philo[(i-1)%NBPHILO] != MANGE) {
        etat_philo[i] = MANGE;
        sem_down(sem_philo[i]);
    }
}
void prendre_fourchette( int i){
    sem_down(sem_tab);
    etat_philo[i]=AFAIM;
    test_mange(i);
    sem_up(sem_tab);
    sem_up(sem_philo[i]);
}

void poser_fourchette(int i){
    sem_down(sem_tab);
    etat_philo[i]=PENSE;
    test_mange((i+1)%NBPHILO);
    sem_up(sem_tab);

}*/
int start_kernel(void) {
    malloc_init((void *) HEAP_START);
    init_hw();
    start_philo();
    start_sched();
    return 0;
}

/* vim: set ft=c et sw=4 sts=4 */

