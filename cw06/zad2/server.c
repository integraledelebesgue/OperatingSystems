#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<mqueue.h>

#include "safe_functions.h"
#include "messages.h"
#include "logs.h"
#include "client_storage.h"


#define MAX_MSG_COUNT 100

client_storage storage;
int running = 1;


void interrupt_handler(int _sig) {
    client_storage_send_all(&storage, "shutdown", 0);
    puts("Waiting for all users to disconnect...");
    while (client_storage_active(&storage) > 0);
    running = 0;
}


void notify_handler(int _sig, siginfo_t* info, void* _context) {
    mqd_t mqd = info->si_value.sival_int;
    msg_t msg = (msg_t) {};

    puts("Received notification. Reading messages..."); 

    while (mq_receive(mqd, (char*)&msg, sizeof(msg), NULL) != -1) {
        printf("Received message: [%ld] %s\n", msg.sender, msg.text);
        log_to_file(msg.text);
    }
    perror("mq_receive");
}


int main(const int argc, const char** argv) {
    if (argc != 2) {
        printf("Wrong arguments. Usage: %s <queue_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct mq_attr attr = {};
    attr.mq_msgsize = sizeof(msg_t);
    attr.mq_maxmsg = MAX_MSG_COUNT;

    mqd_t mqd = safe_mq_open(argv[1], O_RDONLY | O_CREAT | O_NONBLOCK, 0666, &attr);

    struct sigevent sigev = {
        .sigev_notify = SIGEV_SIGNAL,
        .sigev_signo = SIGUSR1,
        .sigev_value = (sigval_t) {
            .sival_int = mqd
        }
    };

    safe_mq_notify(mqd, &sigev);

    safe_sigaction(SIGINT, interrupt_handler, 0);
    safe_sigaction_info(SIGUSR1, notify_handler, SA_SIGINFO);

    set_log_file("log.txt");

    storage = (client_storage) {};
    client_storage_init(&storage);

    log_to_file("Server started");

    printf("Server started. Queue name: %s\n", argv[1]);

    while (running) {
        pause();
    }

    log_to_file("Server shutdown");

    safe_mq_close(mqd);
    safe_mq_unlink(argv[1]);
    close_log_file();
    client_storage_destroy(&storage);

    puts("Server stopped");

    return 0;
}