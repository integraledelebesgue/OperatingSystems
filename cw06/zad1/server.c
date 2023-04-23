#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<sys/msg.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<pthread.h>
#include<semaphore.h>
#include<signal.h>

#include "safe_functions.h"
#include "messages.h"
#include "client_storage.h"
#include "logs.h"
#include "message_processing.h"


sem_t mem_lock;

pthread_t listener_tid;
pthread_t processor_tid;
client_storage storage;


void shut_down() {
    client_storage_send_all(&storage, "shutdown");
    
    puts("Waiting for all users to disconnect...");
    while (client_storage_active(&storage) > 0);

    pthread_cancel(listener_tid);
    pthread_cancel(processor_tid);
}


void interrupt_handdler(int _sig) {
    printf("Do you want to shut the server down? (y/n) ");
    char choice;
    scanf("%c", &choice);

    if (choice == 'y')
        shut_down();
}


int main(const int argc, const char** argv) {
    key_t key = safe_ftok("./server.c", 1);

    int msqid = safe_msgget(key, IPC_CREAT | S_IREAD | S_IWRITE | 0666);

    mem_lock = (sem_t) {};
    storage = (client_storage) {};

    client_storage_init(&storage);

    set_log_file("./log.txt");
    log_to_file(-1, "Server started");

    safe_semaphore(&mem_lock);
    safe_sigaction(SIGINT, interrupt_handdler);

    listener_tid = safe_spawn((void* (*)(void *))listen, (void*)msqid);
    processor_tid = safe_spawn((void* (*)(void *))process, &storage);

    printf("Server started. Message queue key: %d\n", key);

    pthread_join(listener_tid, NULL);
    pthread_join(processor_tid, NULL);

    safe_msgctl(msqid, IPC_RMID, NULL);
    log_to_file(-1, "Server stopped");
    puts("Shutting down...");

    return 0;
}
