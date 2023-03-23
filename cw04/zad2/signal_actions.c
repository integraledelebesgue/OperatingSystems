#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>

#ifndef SIGNAL
#define SIGNAL SIGUSR1
#endif

#ifndef MAX_DEPTH
#define MAX_DEPTH 3
#endif


#define SAFE_SIGACTION(sigaction_struct) {\
    if (sigaction(SIGNAL, &sigaction_struct, NULL) == -1) {\
        perror("Sigaction");\
        exit(EXIT_FAILURE);\
    }\
}


#define ultra_free(ptr) {\
    free(ptr);\
    ptr = NULL;\
}\


void info_handler(int sig, siginfo_t* siginfo) {
    printf(
        "Received signal %d from process %d;\n\tEffective user ID: %d\n\tSignal code: %d\n\tSignal value: %d\n", 
        sig, 
        siginfo->si_pid,
        siginfo->si_uid,
        siginfo->si_code,
        siginfo->si_value.sival_int
    );
}


void set_flag_SIGINFO() {
    struct sigaction action;

    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = (void (*)(int, siginfo_t*, void *))info_handler;
    sigemptyset(&action.sa_mask);

    SAFE_SIGACTION(action)
}


void rec_handler(int sig) { 
    static int* depth = NULL;
    if (!depth) depth = calloc(1, sizeof(int));

    printf("Handling signal %d on depth %d\n", SIGNAL, *depth);
    
    if ((*depth)++ < MAX_DEPTH - 1)
        raise(SIGNAL);
    else 
        ultra_free(depth);
}


void set_flag_NODEFER() {
    struct sigaction action;

    action.sa_flags = SA_NODEFER;
    action.sa_handler = rec_handler;
    sigemptyset(&action.sa_mask);

    SAFE_SIGACTION(action)
}


void oneshot_handler(int sig) {
    printf("Received signal %d in oneshot handler\nRaising the signal again - the default handler will terminate the process\n", SIGNAL);
    raise(SIGNAL);
}


void set_flag_ONESHOT() {
    struct sigaction action;

    action.sa_flags = SA_ONESHOT;
    action.sa_handler = oneshot_handler;
    sigemptyset(&action.sa_mask);

    SAFE_SIGACTION(action)
}


int main(const int argc, const char** argv) {
    puts("----- Testing SIGINFO flag -----");
    set_flag_SIGINFO();
    puts("Queueing a signal with sigval 420...");
    sigqueue(getpid(), SIGNAL, (union sigval){420});
    puts("");

    puts("----- Testing NODEFER flag -----");
    set_flag_NODEFER();
    puts("Raising a signal - a recursive handle is being called");
    raise(SIGNAL);
    puts("");
    
    puts("----- Testing ONESHOT flag -----");
    set_flag_ONESHOT();
    raise(SIGNAL);
    puts("");

    return 0;
}