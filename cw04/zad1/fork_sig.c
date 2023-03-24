#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<string.h>
#include<signal.h>
#include<errno.h>


const static volatile void* ign;
#define __ignore__ ign =

#define SIGNAL SIGUSR1


#define THROW_ARG() {\
    fprintf(stderr, "Wrong arguments\n");\
    exit(EXIT_FAILURE);\
}\


#define SAFE_MASK(mask_set) {\
    if (sigprocmask(SIG_SETMASK, &mask_set, NULL) == -1) {\
        fprintf(stderr, "Error %d; Failed to set mask for signal %d", errno, SIGNAL);\
        exit(-1);\
    }\
}\


#define SAFE_EXEC(execpath, arg, flag) {\
    if (execl(execpath, arg, flag, NULL) == -1) {\
        fprintf(stderr, "Error %d; Failed to execute file \'%s\'\n", errno, execpath);\
    }\
}\


#define SAFE_FORK(pid) {\
    pid = fork();\
    if (pid == -1) {\
        perror("Failed to fork");\
        exit(EXIT_FAILURE);\
    }\
}\


typedef enum {
    NO_ERROR,
    SETUP_ERROR,
    RUNTIME_ERROR
} err;


typedef enum {
    IGNORE,
    HANDLER,
    MASK,
    PENDING,
    NOTHING
} command;


static const char* args[] = {
    "ignore",
    "handler",
    "mask",
    "pending"
};


void handler(int sig) {
    __ignore__ signal(SIGNAL, handler);
    printf("Received signal %d in %d\n", sig, getpid());
}


void set_ignore() {
    __ignore__ signal(SIGNAL, SIG_IGN);
}


void set_handler() {
    __ignore__ signal(SIGNAL, handler);    
}


void set_mask() {
    sigset_t mask_set;

    sigemptyset(&mask_set);
    sigaddset(&mask_set, SIGNAL);

    SAFE_MASK(mask_set)
}


void check_pending() {
    sigset_t pending_set;
    sigpending(&pending_set);

    printf(
        "Signal %d %s pending\n",
        SIGNAL,
        sigismember(&pending_set, SIGNAL) ? "is" : "is not"
    );
}


void process(command cmd) {
    switch (cmd) {
        case IGNORE:
            set_ignore();
            return;

        case HANDLER:
            set_handler();
            return;

        case MASK:
        case PENDING:
            set_mask();
            return;

        default:
            break;
    }
}


void test(command cmd) {
    printf("Raising signal %d in main process... (PID %d)\n", SIGNAL, getpid());
    
    raise(SIGNAL);
    
    printf("Main process: ");
    check_pending();

    fflush(stdout);

    pid_t pid;
    SAFE_FORK(pid)

    if (pid)
        SAFE_EXEC("./other.out", "other.out", cmd == PENDING ? "1" : "0")   
    
    else {
        if (cmd != PENDING) {
            printf("Raising signal %d in child process... (PID %d)\n", SIGNAL, getpid());
            raise(SIGNAL);
        }

        printf("Child process: ");
        check_pending();
    }
}


void parse_arg(const char* arg, command* cmd) {
    for(*cmd = 0; *cmd < 4 && strcmp(arg, args[*cmd]); (*cmd)++);
    if (*cmd == 4) THROW_ARG()
}


int main(const int argc, const char** argv) {
    if (argc != 2) THROW_ARG()

    command cmd;
    parse_arg(argv[1], &cmd);

    printf("Testing for option \'%s\':\n", args[cmd]);

    process(cmd);
    test(cmd);

    return 0;
}