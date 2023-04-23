#ifndef SAFE_FUNCTIONS
#define SAFE_FUNCTIONS

#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<sys/msg.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<semaphore.h>

void safe_sigaction(int signo, void (*handler)(int));
void safe_semaphore(sem_t* sem);
void safe_mutex(pthread_mutex_t* mtx);
pthread_t safe_spawn(void* (*routine)(void*), void* args);
key_t safe_ftok(const char* path, const int id);
ssize_t safe_msgrcv(const int msgid, void* msgp, const size_t msgsz, const long msgtyp, const int msgflg);
int safe_msgctl(const int msgid, const int cmd, struct msqid_ds* buf);
int safe_msgsnd(const int msgid, const void* msgp, const size_t msgsz, const int msgflg);
int safe_msgget(const key_t key, const int flags);

#endif