#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>
#include <sys/types.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<sys/fcntl.h>
#include<semaphore.h>

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
    int shm_desc = safe_shmopen(SHM_NAME, O_CREAT | O_RDWR, S_IREAD | S_IWRITE);
    void* shm_seg = safe_mmap(NULL, sizeof(queue_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_desc, 0);
    safe_ftruncate(shm_desc, sizeof(queue_t));

    queue_t* queue = (queue_t*) shm_seg;
    queue_init(queue);
    
    sem_t* customer_wait_sem = safe_sem_open(WAIT_SEM_NAME, O_CREAT, S_IREAD | S_IWRITE, MAX_WAITING);
    sem_t* customer_get_sem = safe_sem_open(GET_SEM_NAME, O_CREAT, S_IREAD | S_IWRITE, 0);
    sem_t* chair_sem = safe_sem_open(CHAIR_SEM_NAME, O_CREAT, S_IREAD | S_IWRITE, N_CHAIRS);

    puts("Server started");
    
    suspend();

    safe_munmap(shm_seg, sizeof(queue_t));
    safe_shm_unlink(SHM_NAME);

    safe_sem_close(customer_wait_sem);
    safe_sem_close(customer_get_sem);
    safe_sem_close(chair_sem);

    return 0;
}
