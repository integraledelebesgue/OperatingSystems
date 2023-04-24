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
#include<time.h>

#include "safe_functions.h"
#include "common.h"


int main(const int argc, const char** argv) {
    int shm_desc = safe_shmopen(SHM_NAME, O_RDWR, S_IREAD | S_IWRITE);
    void* shm_seg = safe_mmap(NULL, sizeof(queue_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_desc, 0);
    safe_ftruncate(shm_desc, sizeof(queue_t));

    queue_t* queue = (queue_t*) shm_seg;
    queue_init(queue);
    
    sem_t* customer_wait_sem = safe_sem_open(WAIT_SEM_NAME, 0, S_IREAD | S_IWRITE, MAX_WAITING);
    sem_t* customer_get_sem = safe_sem_open(GET_SEM_NAME, 0, S_IREAD | S_IWRITE, 0);
    sem_t* chair_sem = safe_sem_open(CHAIR_SEM_NAME, 0, S_IREAD | S_IWRITE, N_CHAIRS);

    int pid = getpid();

    struct timespec timeout = (struct timespec) {.tv_sec = 5};

    hairstyle_t hairstyle;

    while (1) {
        printf("[Barber %d] is waiting for a customer\n", pid);

        timespec_get(&timeout, TIME_UTC);
        timeout.tv_sec += 5;

        if (sem_timedwait(customer_get_sem, &timeout) == -1) {
            printf("[Barber %d] timed out\n", pid);
            break;
        }

        sem_post(customer_wait_sem);

        sem_wait(chair_sem);

        queue_get(queue, &hairstyle);

        printf("[Barber %d] is doing hairstyle no. %ld for customer %d... ", pid, hairstyle.hairstyle, hairstyle.pid);
        fflush(stdout);

        sleep(hairstyle.hairstyle);

        printf("done!\n");

        kill(hairstyle.pid, SIGDONE);

        sem_post(chair_sem);
    }

    safe_munmap(shm_seg, sizeof(queue_t));

    return 0;
}