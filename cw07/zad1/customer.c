#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include<sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "safe_functions.h"
#include "common.h"


int pid;


void handler(int _sig) {
    printf("[Customer %d] gets their hairstyle done\n", pid);
}


int main(const int argc, const char** argv) {
    if (argc != 3) {
        printf("Wrong arguments. Usage: %s <shared memory segment id> <semaphore set id>", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    pid = getpid();

    const int mem_id = atoi(argv[1]);
    const int sem_id = atoi(argv[2]);

    void* mem_seg = shm_attach(
        mem_id,
        S_IWRITE
    );

    sigset_t wait;
    sigemptyset(&wait);

    signal(SIGDONE, handler);

    queue_t* queue = (queue_t*) mem_seg;

    hairstyle_t hairstyle = {.hairstyle = 1 + rand() % 5, .pid = pid};

    if (sem_trywait(sem_id, customer_wait) == -1)
        goto exit;

    sem_post(sem_id, customer_get);
    queue_put(queue, hairstyle);
    
    printf("[Customer %d] chooses hairstyle no. %ld and waits\n", pid, hairstyle.hairstyle);

    sigsuspend(&wait);

    exit:
    printf("[Customer %d] leaves\n", pid);
    shm_detach(mem_seg);

    return 0;
}