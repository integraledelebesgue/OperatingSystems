#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/resource.h>
#include<sys/types.h>
#include<sys/wait.h>

int main(const int argc, const char** argv) {
    if (argc != 2) {
        puts("Wrong arguments");
        exit(EXIT_FAILURE);
    }

    int n_children = atoi(argv[1]);
    pid_t child_pid;

    for (int i = 0; i < n_children; i++) {
        child_pid = fork();

        if (!child_pid) {
            printf("ID: %d, PPID: %d\n", getpid(), getppid());
            exit(EXIT_SUCCESS);
        }
    }

    while((child_pid = wait(NULL)) > 0);

    printf("Created total number of %d children processes\n", n_children);
    
    return 0;

}