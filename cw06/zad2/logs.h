#ifndef LOGS
#define LOGS

#include "messages.h"

#ifndef MAX_DATETIME_LEN
#define MAX_DATETIME_LEN 30
#endif


void set_log_file(const char* fpath);
void close_log_file();
void log_to_file(const char* message);


#endif