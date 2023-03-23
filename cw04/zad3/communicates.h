#ifndef COMMUNICATES
#define COMMUNICATES

#include<string.h>

#define min(x, y) x < y ? x : y

static const char* cmd_strings[] = {
    "write",
    "time",
    "cmd_count",
    "time_loop",
    "quit"
};


typedef enum {
    NOTHING,
    WRITE,
    TIME,
    CMD_COUNT,
    TIME_LOOP,
    QUIT,
} command;


typedef enum {
    RECEIVED,
    REJECTED,
    TERMINATED
} message;


command verify(int sigval) {
    if (sigval < 0)
        return NOTHING;

    return min(sigval, QUIT);       
}


command parse(const char* cmd_string) {
    size_t i;
    for(i = 0; i < 5 && strcmp(cmd_string, cmd_strings[i]); i++);
    return i < 5 ? i + 1 : NOTHING;
}

#endif