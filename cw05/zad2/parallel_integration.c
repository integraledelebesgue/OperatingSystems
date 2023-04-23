#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<wait.h>
#include<sys/types.h>
#include<semaphore.h>
#include<time.h>
#include<sys/mman.h>
#include<linux/mman.h>

#include "safe_functions.h"


static sem_t exec_time_mutex;

#define safe_semaphore() {           \
    sem_init(&exec_time_mutex, 1, 0);\
    sem_post(&exec_time_mutex);      \
}                                    \


static void* exec_time;

#define safe_shared_memory() {          \
    exec_time = mmap(                   \
        NULL,                           \
        sizeof(time_t),                 \
        PROT_READ | PROT_WRITE,         \
        MAP_SHARED | MAP_ANONYMOUS,     \
        -1,                             \
        0                               \
    );                                  \
    if (!exec_time) exit(EXIT_FAILURE); \
    *(time_t*)exec_time = 0;            \
}                                       \


void compute_partial(double (* fun)(double), const double diff, const int arg_pipe[2], const int return_pipe[2]) {
    double arg;
    double result = 0.0;
    register time_t start_time;
    register time_t end_time;

    start_time = clock();

    while (safe_read(arg_pipe[0], (void*)&arg, sizeof(double)) > 0)
        result += fun(arg) * diff;

    safe_write(return_pipe[1], (void*)&result, sizeof(double));

    end_time = clock();

    sem_wait(&exec_time_mutex);
    *(time_t*)exec_time += end_time - start_time;
    sem_post(&exec_time_mutex);
}


double f(double x) {
    return 4.0 / (x * x + 1);
}


int main(const int argc, const char** argv) {
    if (argc != 3) {
        puts("Wrong arguments");
        exit(EXIT_FAILURE);
    }

    double diff = atof(argv[1]);
    int n  = atoi(argv[2]);
    
    int arg_pipe[2 * n];
    int return_pipe[2 * n];
    
    int i;
    
    safe_semaphore();
    safe_shared_memory();

    for (i = 0; i < 2 * n; i += 2) {
        safe_pipe(arg_pipe + i);
        safe_pipe(return_pipe + i);
    }

    int j = 0;

    for (double arg = 0.0; arg < 1.0; arg += diff, j = (j + 1) % n)
        if (safe_write(arg_pipe[2 * j + 1], (void*)&arg, sizeof(double)) == -1)
            exit(EXIT_FAILURE);

    for (i = 1; i < 2 * n + 1; i += 2)
        safe_close(arg_pipe[i]);

    pid_t pid;

    for (i = 0; i < n; i++)
        switch (pid = fork()) {
            case -1:
                exit(EXIT_FAILURE);
        
            case 0:
                safe_close(return_pipe[2 * i]);

                compute_partial(f, diff, arg_pipe + 2 * i, return_pipe + 2 * i);
                
                safe_close(arg_pipe[0]);
                safe_close(return_pipe[1]);
                
                exit(EXIT_SUCCESS);

            default:
                break;
        }

    while (wait(NULL) > 0);

    double part;
    double result = 0.0;

    for (i = 0; i < 2 * n; i += 2) {
        safe_read(return_pipe[i], (void*)&part, sizeof(double));

        safe_close(return_pipe[i]);
        safe_close(return_pipe[i + 1]);

        result += part;
    }

    printf("For %d processes with %lf accuracy:\n", n, diff);
    printf("Result: %lf\nExecuted in %lf s\n", result, (double)*(time_t*)exec_time / CLOCKS_PER_SEC);

    sem_destroy(&exec_time_mutex);
    munmap(exec_time, sizeof(size_t));

    return 0;
}