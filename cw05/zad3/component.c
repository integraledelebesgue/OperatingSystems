#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<sys/fcntl.h>
#include<unistd.h>
#include<time.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<semaphore.h>


#include "../zad2/safe_functions.h"

double f(double x) {
    return 4.0 / (x * x + 1.0);
}


int main(const int argc, const char** argv) {  // args: integration diff, shared memory object name, shared memory lock name
    double diff = atof(argv[1]);
    double arg;
    double result = 0.0;
    register time_t start_time;
    register time_t end_time;

    start_time = clock();

    while (scanf("%lf", &arg) > 0)
        result += f(arg) * diff;

    end_time = clock();

    int pipe_desc;
    pipe_desc = safe_open("/tmp/return_pipe", O_WRONLY | O_APPEND);

    if (pipe_desc == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    safe_write(pipe_desc, (void*)&result, sizeof(double));

    safe_close(pipe_desc);

    int shm_desc = shm_open(argv[2], O_RDWR, S_IREAD | S_IWRITE);
    void* shm = mmap(NULL, sizeof(time_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_desc, 0);
    sem_t* shm_lock = sem_open(argv[3], 0);

    sem_wait(shm_lock);
    *(time_t*)shm += end_time - start_time;
    sem_post(shm_lock);

    munmap(shm, sizeof(time_t));

    return 0;
}
