#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

#define __ignore__ (void)


void _benchmark(char* execpath, char* arg1, char* arg2, char* arg3, char* arg4) {
    char* test_call;
    struct timespec tms;
    register time_t start_time_s;
    register time_t start_time_ns;

    __ignore__ asprintf(&test_call, "%s %s %s %s %s", execpath, arg1, arg2, arg3, arg4);

    __ignore__ timespec_get(&tms, TIME_UTC);
    start_time_s = tms.tv_sec;
    start_time_ns = tms.tv_nsec;

    __ignore__ system(test_call);
    __ignore__ timespec_get(&tms, TIME_UTC);

    printf(
        "Executed in %ld s %ld ns\n",
        tms.tv_sec - start_time_s,
        tms.tv_nsec - start_time_ns 
    );

    free(test_call);
}


int main(int argc, char** argv) {

    int target_argc = atoi(argv[2]);

    _benchmark(
        argv[1], 
        target_argc >= 1 ? argv[3] : "", 
        target_argc >= 2 ? argv[4] : "", 
        target_argc >= 3 ? argv[5] : "", 
        target_argc >= 4 ? argv[6] : ""
    );

    return 0;
}