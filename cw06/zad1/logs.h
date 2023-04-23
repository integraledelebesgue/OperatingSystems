#ifndef LOGS
#define LOGS

#include "messages.h"


void set_log_file(char* fpath);
void log_to_file(const client_t sender, const char* message);


#endif