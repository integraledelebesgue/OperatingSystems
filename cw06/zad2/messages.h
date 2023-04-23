#ifndef MESSAGES
#define MESSAGES

#define MAX_MSG_LEN 1024

typedef long unsigned client_t;

typedef enum {
    NOTHING,
    STOP,
    LIST,
    TO_ALL,
    TO_ONE,
    INIT,

    first_cmd = STOP,
    last_cmd = TO_ONE,
    invalid_cmd = -1,

    first = STOP,
    last = INIT,

    by_priority = -last
} message_type;


typedef struct {
    client_t sender;
    message_type type;
    char text[MAX_MSG_LEN];
} msg_t;

#endif