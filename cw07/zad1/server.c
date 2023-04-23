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
#include "common.h"


void handler(int _) {
    printf("\nServer shutting down...\n");
}


void suspend() {
    sigset_t wait;
    sigemptyset(&wait);
    signal(SIGINT, handler);
    sigsuspend(&wait);
}


int main(const int argc, const char** argv) {
    const key_t mem_key = safe_ftok("./server.c", 0);
    const int mem_id = shm_create(
        mem_key, 
        sizeof(queue_t), 
        0666
    );

    char mem_id_buffer[15];
    sprintf(mem_id_buffer, "%d", mem_id);

    void* mem_seg = shm_attach(
        mem_id,
        S_IWRITE | S_IREAD
    );

    queue_t* queue = (queue_t*) mem_seg;
    queue_init(queue);
    
    const key_t sem_key = safe_ftok("./server.c", 1);
    const int sem_id = safe_semget(
        sem_key,
        3,
        IPC_CREAT | S_IWRITE | S_IREAD
    );

    char sem_id_buffer[15];
    sprintf(sem_id_buffer, "%d", sem_id);

    printf("Memory ID: %d\nSemaphore ID: %d\n", mem_id, sem_id);

    sem_init(sem_id, customer_wait, MAX_WAITING);
    sem_init(sem_id, customer_get, 0);
    sem_init(sem_id, chair, N_CHAIRS);

    suspend();

    shm_detach(mem_seg);
    shm_remove(mem_id);
    semset_remove(sem_id);

    return 0;
}
