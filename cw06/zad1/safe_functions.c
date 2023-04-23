#define _GNU_SOURCE
#include<pthread.h>
#include "safe_functions.h"
#include<errno.h>
#include<semaphore.h>
#include<signal.h>


void safe_sigaction(int signo, void (*handler)(int)) {
    struct sigaction action = {.sa_flags = 0, .sa_handler = handler};

    sigemptyset(&action.sa_mask);
    sigaddset(&action.sa_mask, SIGINT);

    if (sigaction(signo, &action, NULL) == -1)
        exit(EXIT_FAILURE);
}


void safe_semaphore(sem_t* sem) {
    if (sem_init(sem, 0, 0) == -1) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }
    sem_post(sem);
}


void safe_mutex(pthread_mutex_t* mtx) {
    if (pthread_mutex_init(mtx, NULL) == -1) {
        perror("pthread_mutex_init");
        exit(EXIT_FAILURE);
    }
}


pthread_t safe_spawn(void* (*routine)(void*), void* args) {
    pthread_t tid;
    if (pthread_create(&tid, 0, routine, args) == -1) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    return tid;
}


key_t safe_ftok(const char* path, const int id) {
    key_t key = ftok(path, id);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    return key;
}


ssize_t safe_msgrcv(const int msgid, void* msgp, const size_t msgsz, const long msgtyp, const int msgflg) {
    ssize_t result = msgrcv(msgid, msgp, msgsz, msgtyp, msgflg);
    if (result == -1) {
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }
    return result;
}


int safe_msgctl(const int msgid, const int cmd, struct msqid_ds* buf) {
    int result = msgctl(msgid, cmd, buf);
    if (result == -1) {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }
    return result;
}


int safe_msgsnd(const int msgid, const void* msgp, const size_t msgsz, const int msgflg) {
    int result = msgsnd(msgid, msgp, msgsz, msgflg);
    if (result == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }
    return result;
}


int safe_msgget(const key_t key, const int flags) {
    int msgid = msgget(key, flags);
    if (msgid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
    return msgid;
}

