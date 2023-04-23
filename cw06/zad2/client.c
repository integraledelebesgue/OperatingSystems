#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<mqueue.h>
#include<string.h>

#include "safe_functions.h"
#include "messages.h"
#include "client_storage.h"

#define MAX_MSG_COUNT 10

void notification_handler(int signo, siginfo_t* info, void* context) {
    mqd_t mqd = info->si_value.sival_int;
    char msg_buffer[MAX_MSG_LEN];
    unsigned int priority;

    while (mq_receive(mqd, msg_buffer, MAX_MSG_LEN, &priority) != -1) {
        printf("Received message: %s\n, priority: %d", msg_buffer, priority);
    }
}


int main(const int argc, const char** argv) {
    if (argc != 3) {
        printf("Wrong arguments. Usage: %s <server_queue_name> <client_queue_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    mqd_t server_mqd = safe_mq_open(argv[1], O_WRONLY, 0666, NULL);

    mqd_t mqd = safe_mq_open(argv[2], O_RDONLY | O_CREAT | O_NONBLOCK, 0666, NULL);

    struct mq_attr attr = {
        .mq_msgsize = MAX_MSG_LEN,
        .mq_maxmsg = MAX_MSG_COUNT
    };

    safe_mq_setattr(mqd, &attr, NULL);

    struct sigevent sigev = {
        .sigev_notify = SIGEV_SIGNAL,
        .sigev_signo = SIGUSR1,
        .sigev_value = (sigval_t) {
            .sival_int = mqd
        }
    };

    //safe_mq_notify(mqd, &sigev);

    char cmd_buffer[10];
    char text_buffer[MAX_MSG_LEN - 10];
    char msg_buffer[MAX_MSG_LEN];
    int priority;

    msg_t msg = (msg_t) {
        .sender = (client_t) mqd,
        .type = INIT,
        .text = ""
    };

    safe_mq_send(server_mqd, (char*) &msg, sizeof(msg_t), 0);

    while (1) {
        printf("> ");
        scanf("%s", cmd_buffer);

        printf("Message: ");
        scanf("%s", text_buffer);
        
        printf("Priority: ");
        scanf("%d", &priority);

        sprintf(msg_buffer, "%s;%s", cmd_buffer, text_buffer);

        safe_mq_send(server_mqd, msg_buffer, MAX_MSG_LEN, priority);
    }

    return 0;
}