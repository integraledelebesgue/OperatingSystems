#include<stdio.h>
#include<string.h>

#include"../zad1/errors.h"
#include"repl.h"


error read_command(Session* session) {
    if(!fgets(session->command_buffer, COMMAND_BUFFER_SIZE, stdin))
        return COMMAND_ERROR;

    session->command_buffer[strlen(session->command_buffer)-1] = '\0';
    return NO_ERROR;
}


State _translate_command(char* command) {
    int i = 0;
    while(strcmp(command, commands[i]) && i < COMMAND_COUNT) i++;
    return i;
}


error process_command(Session* session) {
    static const char delim[] = " ";
    char* token = strtok(session->command_buffer, delim);

    if(!token) return COMMAND_ERROR;
    if(strlen(token) > COMMAND_MAX_LENGTH) return COMMAND_ERROR;

    session->state = _translate_command(token);

    token = strtok(NULL, delim);

    if(session->state == NOTHING) return NO_ERROR;
    if(no_arg(session->state)) return NO_ERROR;
    if(!no_arg(session->state) && !token) return ARGUMENT_ERROR;

    session->arg.numeric = atoi(token);
    strcpy(session->arg.string, token);

    return NO_ERROR;
}