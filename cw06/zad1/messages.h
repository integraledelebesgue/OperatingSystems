#ifndef MESSAGES
#define MESSAGES

#ifndef MSG_MAX
#define MSG_MAX 1024
#endif

typedef long unsigned client_t;

typedef struct {
    long type;
    char text[MSG_MAX];
} msgbuf;

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

#endif
