#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<signal.h>
#include<unistd.h>
#include<pthread.h>
#include<errno.h>
#include<time.h>


#include"communicates.h"

#define WRITE_BOUND 10

#ifndef SIGNAL
#define SIGNAL SIGUSR1
#endif


#define loop while(1)


#define BREAK_ON_ERROR() {\
    if (errno) {\
        fprintf(stderr, "Error %d: ", errno);\
        perror("");\
        exit(EXIT_FAILURE);\
    }\
}\


static pthread_mutex_t printf_mutex;
static int pipe_desc[2] = {0, 0};
static int cmd_counter = 0;


void handler(int, siginfo_t*, void*);
pthread_t execute(command cmd);
void time_loop();


void sync_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    pthread_mutex_lock(&printf_mutex);
    vprintf(format, args);
    fflush(stdout);
    pthread_mutex_unlock(&printf_mutex);

    va_end(args);
}


void init_default_mask(sigset_t* mask) {
    sigfillset(mask);
    #ifdef DEBUG
    sigdelset(mask, SIGINT);
    #endif
    sigprocmask(SIG_SETMASK, mask, NULL);
    BREAK_ON_ERROR();
}


void init_await_mask(sigset_t* mask) {
    sigfillset(mask);
    sigfillset(mask);
    #ifdef DEBUG
    sigdelset(mask, SIGINT);
    #endif
    sigdelset(mask, SIGNAL);
    BREAK_ON_ERROR();
}


void init_handler() {
    sigset_t default_mask;
    struct sigaction action;
    
    init_default_mask(&default_mask);
    action.sa_mask = default_mask;
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = (void (*)(int, siginfo_t*, void*))handler;

    sigaction(SIGNAL, &action, NULL);
    BREAK_ON_ERROR();
}


void init_pipe() {
    pipe(pipe_desc);
    BREAK_ON_ERROR()
}


void events_loop() {
    sigset_t await_mask;
    init_await_mask(&await_mask);
    init_handler();

    loop {
        sync_printf("Waiting for signal...\n");
        sigsuspend(&await_mask);
    }
}


void handler(int, siginfo_t* siginfo, void*) {
    command cmd = verify(siginfo->si_value.sival_int);

    if (!cmd) {
        sigqueue(siginfo->si_pid, SIGNAL, (union sigval){.sival_int = REJECTED});
        return;
    }

    write(pipe_desc[1], (void*)&cmd, sizeof(int));
    sigqueue(siginfo->si_pid, SIGNAL, (union sigval){.sival_int = RECEIVED});
    BREAK_ON_ERROR();
}


void processor() {
    command cmd = NOTHING;
    command prev_cmd = NOTHING;
    pthread_t task_id = 0;

    loop {
        read(pipe_desc[0], (void*)&cmd, sizeof(int));
        if (task_id) pthread_cancel(task_id);
        BREAK_ON_ERROR();

        if (cmd != prev_cmd) {
            prev_cmd = cmd;
            cmd_counter++;
        }

        if (cmd == QUIT) {
            sync_printf("Closing catcher\n");
            pthread_exit(NULL);
        }
        task_id = execute(cmd);
    }
}


pthread_t execute(command cmd) {
    int i;
    struct tm* tm;
    time_t timer = time(NULL);
    pthread_t time_loop_id;

    switch (cmd) {
        case WRITE:
            for(i = 1; i <= WRITE_BOUND; i++)
                printf("%d\n", i);
            return 0;

        case TIME:
            tm = localtime(&timer);
            sync_printf("%s", asctime(tm));
            return 0;

        case CMD_COUNT:
            sync_printf("Command has changed %d times since the catcher started\n", cmd_counter);
            return 0;

        case TIME_LOOP:
            pthread_create(&time_loop_id, NULL, (void* (*)(void*))time_loop, NULL);
            BREAK_ON_ERROR();
            return time_loop_id;

        default:
            return 0;
    }
}


void time_loop() {
    struct tm* tm;
    time_t timer;

    loop {
        timer = time(NULL);
        tm = localtime(&timer);
        fprintf(stdout, "%s", asctime(tm));
        sleep(1);
    }
}


int main(const int argc, const char** argv) {
    pthread_t loop_id, processor_id;
    
    setbuf(stdout, NULL);
    pthread_mutex_init(&printf_mutex, NULL);

    init_pipe();
    
    printf("Catcher's PID: %d\n", getpid());

    pthread_create(&loop_id, NULL, (void* (*)(void*))events_loop, NULL);
    pthread_create(&processor_id, NULL, (void* (*)(void*))processor, NULL);
    BREAK_ON_ERROR();

    pthread_join(processor_id, NULL);
    pthread_cancel(loop_id);

    return 0;
}
