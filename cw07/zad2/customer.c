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
    int shm_desc = safe_shmopen(SHM_NAME, O_RDWR, S_IREAD | S_IWRITE);
    void* shm_seg = safe_mmap(NULL, sizeof(queue_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_desc, 0);
    safe_ftruncate(shm_desc, sizeof(queue_t));

    queue_t* queue = (queue_t*) shm_seg;
    queue_init(queue);
    
    sem_t* customer_wait_sem = safe_sem_open(WAIT_SEM_NAME, 0, S_IREAD | S_IWRITE, MAX_WAITING);
    sem_t* customer_get_sem = safe_sem_open(GET_SEM_NAME, 0, S_IREAD | S_IWRITE, 0);

    sigset_t wait;
    sigemptyset(&wait);

    signal(SIGDONE, handler);

    pid = getpid();

    hairstyle_t hairstyle = {.hairstyle = 1 + rand() % 5, .pid = pid};

    if (sem_trywait(customer_wait_sem) == -1)
        goto exit;

    sem_post(customer_get_sem);
    queue_put(queue, hairstyle);
    
    printf("[Customer %d] chooses hairstyle no. %ld and waits\n", pid, hairstyle.hairstyle);

    sigsuspend(&wait);

    exit:
    printf("[Customer %d] leaves\n", pid);
    safe_munmap(shm_seg, sizeof(queue_t));
    sem_close(customer_wait_sem);
    sem_close(customer_get_sem);

    return 0;
}