#ifndef SAFE_FUNCTIONS
#define SAFE_FUNCTIONS

#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<sys/mman.h>
#include<sys/fcntl.h>
#include<semaphore.h>


int safe_shmopen(const char* name, const int flag, const mode_t mode);
void* safe_mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
void safe_munmap(const void* seg, const size_t size);
void safe_shm_unlink(const char* name);
void safe_ftruncate(const int desc, const off_t size);

sem_t* safe_sem_open(const char *name, int oflag, mode_t mode, unsigned int value);
void safe_sem_close(sem_t *sem);

#endif 