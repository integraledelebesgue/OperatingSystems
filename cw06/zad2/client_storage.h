#ifndef CLIENT_STORAGE
#define CLIENT_STORAGE

#include <mqueue.h>

#define MAX_CLIENT_COUNT 10


typedef struct node {
    int client_id;
    struct node* next;
} node;


typedef struct {
    mqd_t queues[MAX_CLIENT_COUNT];
    node* deleted;
    int top;
} client_storage;


void client_storage_init(client_storage* storage);
void client_storage_destroy(client_storage* storage);
int client_storage_add(client_storage* storage, const char* client_queue);
void client_storage_delete(client_storage* storage, const client_t client_id);
void client_storage_send(client_storage* storage, const char* message, const int priority, const client_t client_id);
void client_storage_send_all(client_storage* storage, const char* message, const int priority);
int client_storage_contains(client_storage* storage, client_t client);
int client_storage_active(client_storage* storage);

#endif