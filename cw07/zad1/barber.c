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
#include<signal.h>

#include "safe_functions.h"
#include "common.h"


short running = 1;

void handler(int _sig) {
    running = 0;
}


int main(const int argc, const char** argv) {
    if (argc != 3) {
        printf("Wrong arguments. Usage: %s <shared memory segment id> <semaphore set id>", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handler);
    
    const int mem_id = atoi(argv[1]);
    const int sem_id = atoi(argv[2]);

    void* mem_seg = shm_attach(
        mem_id,
        S_IREAD | S_IWRITE
    );

    queue_t* queue = (queue_t*) mem_seg;
    hairstyle_t hairstyle;

    while (running) {
        printf("[Barber %d] is waiting for a customer\n", getpid());
        sem_wait(sem_id, customer_get);
        sem_post(sem_id, customer_wait);

        sem_wait(sem_id, chair);

        queue_get(queue, &hairstyle);

        printf("[Barber %d] is doing hairstyle no. %ld for customer %d... ", getpid(), hairstyle.hairstyle, hairstyle.pid);
        fflush(stdout);

        sleep(hairstyle.hairstyle);

        printf("done!\n");

        kill(hairstyle.pid, SIGDONE);

        sem_post(sem_id, chair);
    }

    shm_detach(mem_seg);

    return 0;
}