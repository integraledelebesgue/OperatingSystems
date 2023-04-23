#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<pthread.h>
#include<mqueue.h>


mqd_t safe_mq_open(const char* name, int oflag, mode_t mode, struct mq_attr* attr) {
    mqd_t mqd = mq_open(name, oflag, mode, attr);
    if (mqd == -1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }
    return mqd;
}


void safe_mq_close(mqd_t mqd) {
    if (mq_close(mqd) == -1) {
        perror("mq_close");
        exit(EXIT_FAILURE);
    }
}


void safe_mq_unlink(const char* name) {
    if (mq_unlink(name) == -1) {
        perror("mq_unlink");
        exit(EXIT_FAILURE);
    }
}


int safe_mq_send(mqd_t mqd, const char* msg, size_t msg_len, unsigned int prio) {
    int ret = mq_send(mqd, msg, msg_len, prio);
    if (ret == -1) {
        perror("mq_send");
        exit(EXIT_FAILURE);
    }
    return ret;
}


ssize_t safe_mq_receive(mqd_t mqd, char* msg, size_t msg_len, unsigned int* prio) {
    ssize_t ret = mq_receive(mqd, msg, msg_len, prio);
    if (ret == -1) {
        perror("mq_receive");
        exit(EXIT_FAILURE);
    }
    return ret;
}


void safe_mq_getattr(mqd_t mqd, struct mq_attr* attr) {
    if (mq_getattr(mqd, attr) == -1) {
        perror("mq_getattr");
        exit(EXIT_FAILURE);
    }
}


void safe_mq_setattr(mqd_t mqd, struct mq_attr* attr, struct mq_attr* old_attr) {
    if (mq_setattr(mqd, attr, old_attr) == -1) {
        perror("mq_setattr");
        exit(EXIT_FAILURE);
    }
}


void safe_mq_notify(mqd_t mqd, const struct sigevent* sevp) {
    if (mq_notify(mqd, sevp) == -1) {
        perror("mq_notify");
        exit(EXIT_FAILURE);
    }
}


void safe_sigaction(int signo, void (*handler)(int), int flags) {
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = flags;
    if (sigaction(signo, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

void safe_sigaction_info(int signo, void (*handler)(int, siginfo_t*, void*), int flags) {
    struct sigaction sa;
    sa.sa_sigaction = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = flags;
    if (sigaction(signo, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}
