#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#include "messages.h"
#include "client_storage.h"

#ifndef DATETIME_MAX
#define DATETIME_MAX 30
#endif


FILE* logpath;


void set_log_file(char* fpath) {
    if (!(logpath = fopen(fpath, "wa+")))
        exit(EXIT_FAILURE);
}


void log_to_file(const client_t sender, const char* message) {
    static struct tm datetime;
    static time_t timer;
    char log_buffer[MSG_MAX + CLIENT_MAX + DATETIME_MAX];
    char datetime_buffer[DATETIME_MAX];

    timer = time(NULL);
    datetime = *localtime(&timer);

    strftime(datetime_buffer, sizeof(datetime_buffer), "%c", &datetime);
    sprintf(log_buffer, "%s - %ld - %s;\n", datetime_buffer, sender, message);

    fwrite((void*)log_buffer, sizeof(char), strlen(log_buffer), logpath);
}
