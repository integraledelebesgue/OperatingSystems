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


int main(const int argc, const char** argv) {
    if (argc != 3) {
        print("Wrnog arguments. Usage: %s <shared memory segment id> <semaphore set id>", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    const int mem_id = atoi(argv[1]);
    const int sem_id = atoi(argv[2]);

    void* mem_seg = safe_shmat(
        mem_id,
        S_IWRITE | S_IREAD
    );

    safe_shmdt(mem_seg);

    return 0;
}