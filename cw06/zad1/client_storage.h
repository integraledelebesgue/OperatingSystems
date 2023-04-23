#ifndef CLIENT_STORAGE
#define CLIENT_STORAGE

#include<stdlib.h>
#include "messages.h"

#ifndef CLIENT_MAX
#define CLIENT_MAX 10
#endif

typedef struct _node {
    client_t client_id;
    struct _node* next;
} node;


typedef struct _client_storage {
    int queues[CLIENT_MAX];
    size_t top;
    node* deleted;
} client_storage;


void client_storage_init(client_storage* storage);
void client_storage_destroy(client_storage* storage);
int client_storage_add(client_storage* storage, const key_t client_key);
void client_storage_delete(client_storage* storage, const client_t client_id);
void client_storage_send(client_storage* storage, const char* message, const client_t client_id);
void client_storage_send_all(client_storage* storage, const char* message);
int client_storage_contains(client_storage* storage, client_t client);
int client_storage_active(client_storage* storage);

#endif