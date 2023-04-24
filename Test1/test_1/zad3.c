#define _GNU_SOURCE
#include<unistd.h>
#include<sys/types.h>

#include "zad3.h"

void readwrite(int pd, size_t block_size);

void createpipe(size_t block_size)
{
    int pd[2];
    if (pipe(pd) == -1)
        exit(EXIT_FAILURE);

    check_pipe(pd);
    check_write(pd, block_size, readwrite);
}

void readwrite(int write_pd, size_t block_size)
{
    FILE* file = fopen("unix.txt", "r");
    if (!file)
        exit(EXIT_FAILURE);

    size_t size;
    char buffer[block_size];

    while((size = fread((void*)buffer, 1, block_size, file)) > 0)
        write(write_pd, (void*)buffer, size);

    fclose(file);
}

int main()
{
    srand(42);
    size_t block_size = rand() % 128;
    createpipe(block_size);

    return 0;
}