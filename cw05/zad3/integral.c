#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<time.h>
#include<sys/fcntl.h>
#include<sys/mman.h>
#include<semaphore.h>
#include<errno.h>


#include "../zad2/safe_functions.h"

#define SHM_LOCK "/integral_lock"
#define SHM_NAME "/integral_shm"


#define safe_fifo(path, mode) {\
    if (mknod(path, mode, S_IFIFO) == -1) {\
        perror("mknod");\
        exit(EXIT_FAILURE);\
    }\
}\


int main(const int argc, const char** argv) {
    if (argc != 3) {
        puts("Wrong arguments");
        exit(EXIT_FAILURE);
    }

    double diff = atof(argv[1]);
    int n = atoi(argv[2]);
    int i;

    safe_fifo("/tmp/return_pipe", S_IREAD | S_IWRITE);

    int shm_desc = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IREAD | S_IWRITE);
    ftruncate(shm_desc, sizeof(time_t));
    void* shm = mmap(NULL, sizeof(time_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_desc, 0);

    sem_t* shm_lock = sem_open(SHM_LOCK, O_CREAT, S_IREAD | S_IWRITE, 0);
    sem_post(shm_lock);

    if (errno)
        exit(EXIT_FAILURE);

    char *call_buffer;
    (void)asprintf(&call_buffer, "./component.out %lf %s %s", diff, SHM_NAME, SHM_LOCK);

    FILE* inputs[n];
    for (i = 0; i < n; i++)
        inputs[i] = popen(call_buffer, "w");

    double arg;
    for (arg = 0.0, i = 0; arg < 1.0; arg += diff, i = (i + 1) % n)
        fprintf(inputs[i], "%lf", arg);

    for (i = 0; i < n; i++)
        fclose(inputs[i]);

    int return_desc = safe_open("/tmp/return_pipe", O_RDONLY);

    double part;
    double result = 0.0;
    while (safe_read(return_desc, (void*)&part, sizeof(double)) > 0)
        result += part;

    printf("For %d processes with %lf accuracy:\n", n, diff);
    printf("Result: %lf\n", result);
    printf("Executed in %lf s\n\n", (double)*(time_t*)shm / CLOCKS_PER_SEC);

    free(call_buffer);
    safe_close(return_desc);
    munmap(shm, sizeof(time_t));
    shm_unlink(SHM_NAME);
    sem_destroy(shm_lock);
    remove("/tmp/return_pipe");
    
    return 0;
}
