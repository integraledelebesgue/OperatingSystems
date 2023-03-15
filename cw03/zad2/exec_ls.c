#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>


int main(const int argc, const char** argv) {
    if (argc != 2) {
        puts("Wrong arguments");
        exit(EXIT_FAILURE);
    }

    printf("Running program: %s\n", argv[0]);
    printf("Executing /bin/ls on %s:\n", argv[1]);
    fflush(stdout);

    char* path = realpath(argv[1], NULL);

    execl("/bin/ls", "ls", path, (char*)NULL);

    free(path);

    return 0;
}