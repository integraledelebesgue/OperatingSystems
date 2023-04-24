#include<errno.h>
#include "safe_functions.h"


int safe_shmopen(const char* name, const int flag, const mode_t mode) {
    int desc = shm_open(name, flag, mode);
    if (desc == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    return desc;
}


void* safe_mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset) {
    void* seg = mmap(addr, len, prot, flags, fd, offset);
    if (seg == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    return seg;
}


void safe_munmap(const void* seg, const size_t size) {
    if (munmap((void*) seg, size) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }
}


void safe_shm_unlink(const char* name) {
    if (shm_unlink(name) == -1) {
        perror("shm_unlink");
        exit(EXIT_FAILURE);
    }
}


void safe_ftruncate(const int desc, const off_t size) {
    if (ftruncate(desc, size) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
}


sem_t* safe_sem_open(const char *name, int oflag, mode_t mode, unsigned int value) {
    sem_t* sem = sem_open(name, oflag, mode, value);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    return sem;
}


void safe_sem_close(sem_t *sem) {
    if (sem_close(sem) == -1) {
        perror("sem_close");
        exit(EXIT_FAILURE);
    }
}
