#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>

#define SIGNAL SIGUSR1

void check_pending() {
    sigset_t pending_set;
    sigpending(&pending_set);

    printf(
        "Signal %d %s pending\n",
        SIGNAL,
        sigismember(&pending_set, SIGNAL) ? "is" : "is not"
    );
}


int main(const int argc, const char** argv) {
    int pending_flag = atoi(argv[1]);
    if (!pending_flag) {
        printf("Raising signal in exec's process... (PID %d)\n", getpid());
        fflush(stdout);
        raise(SIGNAL);
    }

    printf("Exec's process: ");
    check_pending();

    return 0;
}