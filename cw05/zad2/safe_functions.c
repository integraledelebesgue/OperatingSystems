#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/fcntl.h>

#include "safe_functions.h"


int safe_open(const char* filepath, int mode) {
    int out;
    if ((out = open(filepath, mode)) == -1)
        exit(EXIT_FAILURE);
    
    return out;
}


void safe_close(int desc) {
    if (close(desc) == -1) {
        perror("Descriptor");
        exit(EXIT_FAILURE);
    }                          
}                              


void safe_pipe(int pipe_desc[2]) {
    if (pipe(pipe_desc) == -1)
        exit(EXIT_FAILURE);
}


int safe_read(int desc, void* buffer, size_t size) {
    int out;
    if((out = read(desc, buffer, size)) == -1)
        exit(EXIT_FAILURE);

    return out;
}


int safe_write(int desc, void* buffer, size_t size) {
    int out;
    if((out = write(desc, buffer, size)) == -1)
        exit(EXIT_FAILURE);

    return out;
}
