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

#define WRITE_BOUND 100

#ifndef SIGNAL
#define SIGNAL SIGUSR1
#endif


void handler(int, siginfo_t*, void*);
pthread_t execute(command cmd);
void time_loop();


#define loop while(1)


static int pipe_desc[2];

#define init_pipe() {   \
    pipe(pipe_desc);    \
    sync_error_check()  \
}                       \


static pthread_mutex_t errno_mutex;
static pthread_mutex_t printf_mutex;

#define init_mutex() {                        \
    pthread_mutex_init(&printf_mutex, NULL);  \
    pthread_mutex_init(&errno_mutex, NULL);   \
    sync_error_check();                       \
}                                             \


#define init_handler() {                                              \
    sigset_t default_mask;                                            \
    struct sigaction action;                                          \
    init_default_mask(&default_mask);                                 \
    action.sa_mask = default_mask;                                    \
    action.sa_flags = SA_SIGINFO;                                     \
    action.sa_sigaction = (void (*)(int, siginfo_t*, void*))handler;  \
    sigaction(SIGNAL, &action, NULL);                                 \
    sync_error_check();                                               \
}                                                                     \


#define sync_error_check() {                   \
    pthread_mutex_lock(&errno_mutex);          \
    if (errno){                                \
        fprintf(stderr, "Error %d: ", errno);  \
        perror("");                            \
        exit(EXIT_FAILURE);                    \
    }                                          \
    pthread_mutex_unlock(&errno_mutex);        \
}                                              \


void sync_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    pthread_mutex_lock(&printf_mutex);
    vprintf(format, args);
    pthread_mutex_unlock(&printf_mutex);

    va_end(args);
}


void init_default_mask(sigset_t* mask) {
    sigfillset(mask);
    
    #ifdef DEBUG
    sigdelset(mask, SIGINT);
    #endif
    
    sync_error_check();
}


void init_await_mask(sigset_t* mask) {
    sigfillset(mask);
    sigdelset(mask, SIGNAL);
    
    #ifdef DEBUG
    sigdelset(mask, SIGINT);
    #endif
    
    sync_error_check();
}


_Noreturn void listener() {
    sigset_t await_mask;
    sigset_t default_mask;

    init_default_mask(&default_mask);
    init_await_mask(&await_mask);

    sigprocmask(SIG_SETMASK, &default_mask, NULL);
    
    loop {
        sigsuspend(&await_mask);
    }
}


void handler(int, siginfo_t* siginfo, void*) {
    command cmd = verify(siginfo->si_value.sival_int);

    switch (cmd) {
        case NOTHING:
            sigqueue(siginfo->si_pid, SIGNAL, (union sigval){.sival_int = REJECTED});
            break;

        case QUIT:
            write(pipe_desc[1], (void*)&cmd, sizeof(int));
            sigqueue(siginfo->si_pid, SIGNAL, (union sigval){.sival_int = TERMINATED});
            break;

        default:
            write(pipe_desc[1], (void*)&cmd, sizeof(int));
            sigqueue(siginfo->si_pid, SIGNAL, (union sigval){.sival_int = RECEIVED});
            break;
    }
    
    //sync_error_check();  // Checking for errors here causes program to crash 
    //                        despite having a mutex on errno.
    //                        Must remain unchecked then.
}


void processor() {
    command cmd = NOTHING;
    pthread_t task_id = 0;
    sigset_t mask;

    init_default_mask(&mask);
    sigprocmask(SIG_SETMASK, &mask, NULL);
    sync_error_check();

    loop {
        read(pipe_desc[0], (void*)&cmd, sizeof(int));

        if (task_id) pthread_cancel(task_id);

        if (cmd == QUIT) {
            sync_printf("Terminating processor thread\n");
            pthread_exit(NULL);
        }

        task_id = execute(cmd);
    }
}


pthread_t execute(command cmd) {
    static int i;
    static struct tm* tm;
    static time_t timer;
    static command prev_cmd;
    static int cmd_counter;
    static pthread_t time_loop_id;

    if (cmd != prev_cmd) {
        prev_cmd = cmd;
        cmd_counter++;
    }

    switch (cmd) {
        case WRITE:
            for(i = 1; i <= WRITE_BOUND; i++)
                printf("%d\n", i);
            return 0;

        case TIME:
            timer = time(NULL);
            tm = localtime(&timer);
            sync_printf("%s", asctime(tm));
            return 0;

        case CMD_COUNT:
            sync_printf("Command has changed %d times since the catcher started\n", cmd_counter);
            return 0;

        case TIME_LOOP:
            pthread_create(&time_loop_id, NULL, (void* (*)(void*))time_loop, NULL);
            return time_loop_id;

        default:
            return 0;
    }
}


_Noreturn void time_loop() {
    static struct tm* tm;
    static time_t timer;
    static sigset_t mask;

    init_default_mask(&mask);
    sigprocmask(SIG_SETMASK, &mask, NULL);
    sync_error_check();

    loop {
        timer = time(NULL);
        tm = localtime(&timer);
        sync_printf("%s", asctime(tm));
        sleep(1);
    }
}


int main(const int argc, const char** argv) {
    pthread_t listener_id, processor_id;
    sigset_t mask;
    
    setbuf(stdout, NULL);

    init_mutex();
    init_pipe();
    init_handler();
    init_default_mask(&mask);

    sigprocmask(SIG_SETMASK, &mask, NULL);
    sync_error_check();

    printf("Catcher's PID: %d\n", getpid());

    pthread_create(&listener_id, NULL, (void* (*)(void*))listener, NULL);
    pthread_create(&processor_id, NULL, (void* (*)(void*))processor, NULL);
    sync_error_check();

    pthread_join(processor_id, NULL);
    pthread_cancel(listener_id);

    sync_error_check();

    return 0;
}
