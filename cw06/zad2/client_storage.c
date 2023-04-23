#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<pthread.h>
#include<mqueue.h>
#include<string.h>

#include "safe_functions.h"
#include "messages.h"
#include "client_storage.h"


void client_storage_init(client_storage* storage) {
    storage->deleted = NULL;
    storage->top = 0;
    memset(storage->queues, -1, MAX_CLIENT_COUNT * sizeof(int));
}


void _node_deallocate(node* node) {
    if (node->next)
        _node_deallocate(node->next);

    free(node);
}


void client_storage_destroy(client_storage* storage) {
    for (int i = 0; i < storage->top; i++) {
        if (storage->queues[i] != -1)
            safe_mq_close(storage->queues[i]);
    }

    if (storage->deleted)
        _node_deallocate(storage->deleted);
}


int client_storage_add(client_storage* storage, const char* client_queue) {
    if (storage->deleted != NULL) {
        node* tmp = storage->deleted;
        
        storage->deleted = storage->deleted->next;
        storage->queues[tmp->client_id] = safe_mq_open(client_queue, O_WRONLY, 0666, NULL);
        
        int id = tmp->client_id;
        free(tmp);
        return id;
    }

    storage->queues[storage->top++] = safe_mq_open(client_queue, O_WRONLY, 0666, NULL);
    return storage->top - 1;
}


void client_storage_delete(client_storage* storage, const client_t client_id) {
    storage->queues[client_id] = -1;
    node* tmp = malloc(sizeof(node));
    tmp->client_id = client_id;
    tmp->next = storage->deleted;
    storage->deleted = tmp;
}


void client_storage_send(client_storage* storage, const char* message, const int priority, const client_t client_id) {
    safe_mq_send(storage->queues[client_id], message, MAX_MSG_LEN, priority);
}


void client_storage_send_all(client_storage* storage, const char* message, const int priority) {
    for (size_t i = 0; i < storage->top; i++)
        if (storage->queues[i] > -1)
            client_storage_send(storage, message, i, priority);
}


int client_storage_contains(client_storage* storage, client_t client) {
    return client < MAX_CLIENT_COUNT && storage->queues[client] != -1;
}


int client_storage_active(client_storage* storage) {
    int count = 0;
    for (client_t client = 0; client < MAX_CLIENT_COUNT; client++)
        if (storage->queues[client] > -1)
            count++;

    return count;
}
