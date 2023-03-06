#ifndef REPL
#define REPL
#include<stdlib.h>
#include<stdio.h>

#include"../zad1/memory_blocks.h"
#include"../zad1/errors.h"

#define COMMAND_COUNT 7
#define COMMAND_BUFFER_SIZE FILENAME_MAX + 200
#define COMMAND_MAX_LENGTH 20

#define no_arg(cmd) (cmd < 2)
#define num_arg(cmd) (2 <= cmd && cmd < 5)
#define str_arg(cmd) (5 <= cmd && cmd < 7)


static const char commands[COMMAND_COUNT][COMMAND_MAX_LENGTH] = {
    "destroy",
    "quit",
    "init",
    "show",
    "delete",
    "read",
    "count"
};

typedef enum {
    DESTROY,
    QUIT,
    INIT,
    SHOW,
    DELETE,
    READ,
    COUNT,
    NOTHING
} State;


typedef struct {
    size_t numeric;
    char string[COMMAND_BUFFER_SIZE];
} StateArg;


typedef struct {
    BlockList* block_list;
    short initialized;
    State state;
    StateArg arg;
    char command_buffer[COMMAND_BUFFER_SIZE];
} Session;

#endif