#ifndef SAFE_FUNCTIONS
#define SAFE_FUNCTIONS

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>

/* From man-7 docs: */
union semun {
    int val;    /* Value for SETVAL */
    struct semid_ds* buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short* array;  /* Array for GETALL, SETALL */
    struct seminfo* __buf;  /* Buffer for IPC_INFO */
};


key_t safe_ftok(const char* path, const int id);

int safe_shmget(const key_t key, const size_t size, const int shmflg);
int shm_create(const key_t key, const size_t size, const int flag);
void* shm_attach(const int id, const int flag);
void shm_detach(const void* seg);
void shm_control(const int id, const int cmd, struct shmid_ds* buf);
void shm_remove(const int id);

int safe_semget(const key_t key, const int n_sems, const int flag);
void safe_semop(const int id, struct sembuf* ops, unsigned n_ops);
void sem_wait(const int id, const int num);
int sem_trywait(const int id, const int num);
void sem_post(const int id, const int num);
int safe_semctl(const int id, const int num, const int cmd, union semun arg);
void sem_init(const int id, const int num, const int val);
void semset_remove(const int id);

#endif 