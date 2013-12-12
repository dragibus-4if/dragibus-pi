#define _POSIX_SOURCE
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef LONG_NB_COMPUTATIONS
#  define LONG_NB_COMPUTATIONS 42
#endif

#ifndef LONG_COMPUTATION_TIME
#  define LONG_COMPUTATION_TIME 4200000
#endif

#ifndef LONG_PRIORITY
#  define LONG_PRIORITY 0
#endif

#ifndef SHORT_COMPUTATION_TIME
#  define SHORT_COMPUTATION_TIME 420000
#endif

#ifndef SHORT_POLICY
#  define SHORT_POLICY SCHED_FIFO
#endif

#ifndef SHORT_PRIORITY
#  define SHORT_MAX_PRIORITY sched_get_priority_max(SHORT_POLICY)
#  define SHORT_MIN_PRIORITY sched_get_priority_min(SHORT_POLICY)
#  define SHORT_PRIORITY ((SHORT_MAX_PRIORITY) + (SHORT_MIN_PRIORITY)) >> 1
#endif

#ifndef SHORT_KILL_SIGNAL
#  define SHORT_KILL_SIGNAL SIGINT
#endif

static void _die_silently(int signo) {
    exit(EXIT_SUCCESS);
}

int main(void) {
    /* sémaphore de l'écran pour STDOUT */
    sem_t stdout_lock;
    if (sem_init(&stdout_lock, 1, 1) == -1) {
        perror("initialisation du verrou STDOUT");
        exit(EXIT_FAILURE);
    }

    /* clonage du processus courant */
    pid_t cpid = fork();

    /* gestion des processus */
    if (cpid == -1) { /* erreur */
        perror("fork initial");
        exit(EXIT_FAILURE);
    } else if (cpid == 0) { /* fils: processus "court" */
        /* mise en place de la priorité */
        struct sched_param csched_param;
        csched_param.sched_priority = SHORT_PRIORITY;
        if (sched_setscheduler(cpid, SHORT_POLICY, &csched_param) == -1) {
            perror("sched_setscheduler");
            exit(EXIT_FAILURE);
        }

        /* synchronisation de fin */
        struct sigaction new_action, old_action;
        new_action.sa_handler = _die_silently;
        sigemptyset (&new_action.sa_mask);
        new_action.sa_flags = 0;
        sigaction (SHORT_KILL_SIGNAL, NULL, &old_action);
        if (old_action.sa_handler != SIG_IGN) {
            sigaction (SHORT_KILL_SIGNAL, &new_action, NULL);
        }

        /* calculs */
        for (long long unsigned int i = 0; /* true */; i++) {
            sem_wait(&stdout_lock);
            printf("%i\n", 0);
            fflush(stdout);
            sem_post(&stdout_lock);
            for (size_t j = 0; j < SHORT_COMPUTATION_TIME; j++);
        }
    } else { /* parent: processus "long" */
        /* mise en place de la priorité */
        if (setpriority(PRIO_PROCESS, 0, LONG_PRIORITY) == -1) {
            perror("setpriority");
            kill(cpid, SHORT_KILL_SIGNAL);
            exit(EXIT_FAILURE);
        }

        /* calculs */
        for (size_t i = 0; i < LONG_NB_COMPUTATIONS; i++) {
            sem_wait(&stdout_lock);
            printf("%i\n", 1);
            fflush(stdout);
            sem_post(&stdout_lock);
            for (size_t j = 0; j < LONG_COMPUTATION_TIME; j++);
        }

        /* fin du long: mort du court */
        sem_wait(&stdout_lock);
        sem_post(&stdout_lock);
        if (kill(cpid, SHORT_KILL_SIGNAL) == -1) {
            perror("kill du processus fils");
            exit(EXIT_SUCCESS);
        }
    }
}
