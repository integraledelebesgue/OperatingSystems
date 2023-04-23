#define _GNU_SOURCE
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>

#include "client_storage.h"
#include "safe_functions.h"
#include "messages.h"


void client_storage_init(client_storage* storage) {
    memset(storage->queues, -1, CLIENT_MAX * sizeof(int));
    storage->deleted = NULL;
    storage->top = 0;
}


void _node_deallocate(node* node) {
    if (node->next)
        _node_deallocate(node->next);

    free(node);
}


void client_storage_destroy(client_storage* storage) {
    _node_deallocate(storage->deleted);
}


int client_storage_add(client_storage* storage, const key_t client_key) {
    if (storage->deleted != NULL) {
        node* tmp = storage->deleted;
        
        storage->deleted = storage->deleted->next;
        storage->queues[tmp->client_id] = safe_msgget(client_key, S_IWRITE);
        
        int id = tmp->client_id;
        free(tmp);
        return id;
    }

    storage->queues[storage->top++] = safe_msgget(client_key, S_IWRITE);
    return storage->top - 1;
}


void client_storage_delete(client_storage* storage, const client_t client_id) {
    storage->queues[client_id] = -1;
    node* tmp = malloc(sizeof(node));
    tmp->client_id = client_id;
    tmp->next = storage->deleted;
    storage->deleted = tmp;
}


void client_storage_send(client_storage* storage, const char* message, const client_t client_id) {
    msgbuf msg = {};
    msg.type = 1;
    strcpy(msg.text, message);
    safe_msgsnd(storage->queues[client_id], &msg, MSG_MAX * sizeof(char), 0);
}


void client_storage_send_all(client_storage* storage, const char* message) {
    for (size_t i = 0; i < storage->top; i++)
        if (storage->queues[i] > -1)
            client_storage_send(storage, message, i);
}


int client_storage_contains(client_storage* storage, client_t client) {
    return client < CLIENT_MAX && storage->queues[client] != -1;
}


int client_storage_active(client_storage* storage) {
    int count = 0;
    for (client_t client = 0; client < CLIENT_MAX; client++)
        if (storage->queues[client] > -1)
            count++;

    return count;
}
