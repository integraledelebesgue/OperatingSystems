#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<errno.h>

#include "communicates.h"

#define SIGNAL SIGUSR1


#define THROW_ARG() {\
    fprintf(stderr, "Wrong arguments\n");\
    exit(EXIT_FAILURE);\
}\


#define BREAK_ON_ERROR() {\
    if (errno) {\
        fprintf(stderr, "Error %d ", errno);\
        perror("");\
        exit(EXIT_FAILURE);\
    }\
}\


void handler(int, siginfo_t* siginfo);


void set_default_mask(sigset_t* mask) {
    sigfillset(mask);
    #ifdef DEBUG
    sigdelset(mask, SIGINT);
    #endif
    sigprocmask(SIG_SETMASK, mask, NULL);
    check_for_error();
}


void init_await_mask(sigset_t* mask) {
    sigfillset(mask);
    #ifdef DEBUG
    sigdelset(mask, SIGINT);
    #endif
    sigdelset(mask, SIGNAL);
}


void set_handler() {
    struct sigaction action;
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = (void (*)(int, siginfo_t*, void*))handler;
    set_default_mask(&action.sa_mask);
    sigaction(SIGNAL, &action, NULL);
    check_for_error();
}


size_t parse_args(const int arg_count, const char** args, command* commands) {
    size_t j = 0;
    command cmd;

    for(size_t i = 0; i < arg_count; i++)
        if ((cmd = parse(args[i])))
            commands[j++] = cmd;

    return j;
}


void handler(int, siginfo_t* siginfo) {
    message status = siginfo->si_value.sival_int;

    switch (status) {
        case RECEIVED:
            puts("Command successfully received by catcher");
            return;

        case REJECTED:
            puts("Command rejected by catcher");
            return;

        case TERMINATED:
            fprintf(stderr, "Catcher has terminated; Terminating sender\n");
            exit(EXIT_FAILURE);
    }
}


void send_commands(const pid_t catcher_pid, const command* commands, const size_t cmd_count) {
    sigset_t await_mask;
    init_await_mask(&await_mask);
    set_handler();
    
    for(size_t i = 0; i < cmd_count; i++) {
        printf("Sending %d\n", commands[i]);
        sigqueue(catcher_pid, SIGNAL, (union sigval){.sival_int = commands[i]});
        sigsuspend(&await_mask);
    }
}


int main(const int argc, const char** argv) {
    if (argc < 3) THROW_ARG()

    setbuf(stdout, NULL);

    printf("Sender PID: %d\n", getpid());

    pid_t catcher_pid = atoi(argv[1]);

    command commands[argc];
    size_t cmd_count = parse_args(argc - 2, argv + 2, commands);

    send_commands(catcher_pid, commands, cmd_count);

    return 0;
}