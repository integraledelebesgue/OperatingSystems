#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "safe_functions.h"


#ifndef MAX_WAITING
#define MAX_WAITING 15
#endif

#ifndef N_CHAIRS
#define N_CHAIRS 5
#endif

#ifndef N_BARBERS
#define N_BARBERS 10
#endif

#if (N_BARBERS < N_CHAIRS)
#error "A number of barbers should be equal or greater than the number of chairs"
#endif 


typedef enum {
    customer_wait = 0,
    customer_get = 1,
    chair = 2
} sems;


void spawn_barbers(const int mem_id, const int sem_id) {
    puts("Spawning barbers...");
}


void spawn_customers(const int mem_id, const int sem_id) {
    puts("Spawning customers...");
}


void handler(int _) {
    puts("Server shutting down...");
}


void suspend() {
    sigset_t wait;
    sigemptyset(&wait);
    signal(SIGINT, handler);
    sigsuspend(&wait);
}


int main(const int argc, const char** argv) {
    const key_t mem_key = safe_ftok("./server.c", 0);
    const int mem_id = safe_shmget(
        mem_key, 
        (N_CHAIRS + MAX_WAITING) * sizeof(int), 
        IPC_CREAT | 0666
    );

    void* mem_seg = safe_shmat(
        mem_id,
        S_IWRITE | S_IREAD
    );
    
    const key_t sem_key = safe_ftok("./server.c", 1);
    const int sem_id = safe_semget(
        sem_key,
        3,
        IPC_CREAT | S_IWRITE | S_IREAD
    );

    init_sem(sem_id, customer_wait, MAX_WAITING);
    init_sem(sem_id, customer_get, 0);
    init_sem(sem_id, chair, N_CHAIRS);

    spawn_barbers(mem_id, sem_id);
    spawn_customers(mem_id, sem_id);
    suspend();

    safe_shmdt(mem_seg);
    remove_shm(mem_id);
    remove_semset(sem_id);

    return 0;
}
