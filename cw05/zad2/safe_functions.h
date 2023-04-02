#ifndef SAFE_FUNCTIONS
#define SAFE_FUNCTIONS

#include<stdlib.h>

int safe_open(const char* filepath, int mode);
void safe_close(int desc);
void safe_pipe(int pipe_desc[2]);
int safe_read(int desc, void* buffer, size_t size);
int safe_write(int desc, void* buffer, size_t size);

#endif