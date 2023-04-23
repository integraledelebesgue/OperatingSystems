#ifndef SAFE_FUNCTIONS
#define SAFE_FUNCTIONS

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <mqueue.h>

mqd_t safe_mq_open(const char* name, int oflag, mode_t mode, struct mq_attr* attr);
void safe_mq_close(mqd_t mqd);
void safe_mq_unlink(const char* name);
int safe_mq_send(mqd_t mqd, const char* msg, size_t msg_len, unsigned int prio);
ssize_t safe_mq_receive(mqd_t mqd, char* msg, size_t msg_len, unsigned int* prio);
void safe_mq_getattr(mqd_t mqd, struct mq_attr* attr);
void safe_mq_setattr(mqd_t mqd, struct mq_attr* attr, struct mq_attr* old_attr);
void safe_mq_notify(mqd_t mqd, const struct sigevent* sevp);
void safe_sigaction(int signo, void (*handler)(int), int flags);
void safe_sigaction_info(int signo, void (*handler)(int, siginfo_t*, void*), int flags);

#endif