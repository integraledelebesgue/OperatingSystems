#define _GNU_SOURCE
#include<sys/signal.h>
#include<sys/types.h>
#include<stdlib.h>
#include<stdio.h>
#include "zad2.h"


void mask()
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    check_mask();
}

void process()
{
    pid_t pid;

    switch(pid = fork()) {
        case -1:
            exit(EXIT_FAILURE);
        
        case 0:
            execl("./check_fork", "check_fork", NULL);
            exit(EXIT_SUCCESS);

        default:
            sleep(1);
            kill(pid, SIGUSR1);
            waitpid(pid, NULL, 0);
    }
}

int main()
{
    mask();
    process();

    return 0;
}