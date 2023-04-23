#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#include "logs.h"
#include "messages.h"
#include "client_storage.h"


FILE* logfile;


void set_log_file(const char* fpath) {
    if (!(logfile = fopen(fpath, "wa+")))
        exit(EXIT_FAILURE);
}


void close_log_file() {
    fclose(logfile);
}


void log_to_file(const char* message) {
    static struct tm datetime;
    static time_t timer;
    char log_buffer[MAX_MSG_LEN + MAX_CLIENT_COUNT + MAX_DATETIME_LEN];
    char datetime_buffer[MAX_DATETIME_LEN];

    timer = time(NULL);
    datetime = *localtime(&timer);

    strftime(datetime_buffer, sizeof(datetime_buffer), "%c", &datetime);
    sprintf(log_buffer, "%s - %s;\n", datetime_buffer, message);

    fwrite((void*)log_buffer, sizeof(char), strlen(log_buffer), logfile);
}
