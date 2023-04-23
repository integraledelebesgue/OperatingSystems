#define _GNU_SOURCE
#include<string.h>
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/signal.h>

#include "message_processing.h"
#include "safe_functions.h"
#include "messages.h"
#include "client_storage.h"
#include "logs.h"


message_type shared_type;
client_t shared_sender;
short notification = 0;
char shared_text[MSG_MAX + 1];


void ignore_signals() {
    sigset_t sigset;
    sigfillset(&sigset);
    sigprocmask(SIG_BLOCK, &sigset, NULL);
}


void process(client_storage* storage) {
    client_t sender;
    client_t receiver;
    char* token;
    char text[MSG_MAX + 100];
    char number_buffer[CLIENT_MAX];

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while (1) {
        if (!notification)
            continue;
        notification = 0;

        sem_wait(&mem_lock);

        puts("Processing request!");
        fflush(stdout);

        switch (shared_type) {
            case STOP:
                sender = shared_sender;
                client_storage_delete(storage, sender);
                sem_post(&mem_lock);
                break;

            case INIT:
                key_t receiver_key = atoi(shared_text);
                printf("New client key: %d\n", receiver_key);
                receiver = client_storage_add(storage, receiver_key);
                sem_post(&mem_lock);

                sprintf(text, "%ld", receiver);
                client_storage_send(storage, text, receiver);
                break;

            case LIST:
                sender = shared_sender;
                sem_post(&mem_lock);

                memset(text, 0, MSG_MAX + 1);

                for (client_t client; client < CLIENT_MAX; client++) {
                    if (storage->queues[client] == -1)
                        continue;

                    sprintf(number_buffer, "%d; ", storage->queues[client]);
                    strcat(text, number_buffer);
                }

                client_storage_send(storage, text, sender);
                break;

            case TO_ALL:
                sprintf(text, "(all) %ld: %s", shared_sender, shared_text);
                sem_post(&mem_lock);
                printf("%s\n", text);
                client_storage_send_all(storage, text);
                break;

            case TO_ONE:
                sender = shared_sender;
                strcpy(text, shared_text);
                sem_post(&mem_lock);

                token = strtok(text, ";");
                receiver = atol(token);
                token = strtok(NULL, ";");

                char* msg_text;
                
                asprintf(&msg_text, "(priv) %ld: %s", sender, token);

                printf("%s\n", msg_text);

                if (client_storage_contains(storage, receiver))
                    client_storage_send(storage, msg_text, receiver);

                free(msg_text);
                break;

            default:
                sem_post(&mem_lock);
                break;
        }
    }
}


void listen(int msqid) {
    msgbuf message = {};
    char* token;
    client_t sender;

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while (1) {
        safe_msgrcv(msqid, &message, MSG_MAX * sizeof(char), by_priority, MSG_NOERROR);
        
        token = strtok(message.text, "$");
        sender = atol(token);
        token = strtok(NULL, "$");

        sem_wait(&mem_lock);

        notification = 1;
        shared_sender = sender;
        shared_type = message.type;

        if (token) 
            strcpy(shared_text, token);
        else
            strcpy(shared_text, "");
        
        sem_post(&mem_lock);

        log_to_file(shared_sender, shared_text); 
    }
}
