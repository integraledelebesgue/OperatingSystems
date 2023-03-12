#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<ftw.h>
#include<string.h>
#include<errno.h>

#ifndef OPEN_LIMIT
#define OPEN_LIMIT 100
#endif

#define CONTINUE 0
#define __ignore__ (void)

typedef long long unsigned fsize;

static fsize total_size;

static void inc_size(size_t file_size) {
    total_size += file_size;
}


void get_filename(const char* filepath, char* buffer) {   
    strcpy(buffer, filepath);
    char* token = strtok(buffer, "/");
    char* prev_token;

    while(token) {
        prev_token = token;
        token = strtok(NULL, "/");
    }

    strcpy(buffer, prev_token);
}


int entry_handle(const char* filepath, struct stat* stat_buffer, int typeflag) {
    static char filename_buffer[FILENAME_MAX];

    if (typeflag == FTW_D) {
        printf("%s/:\n", filepath);
        return CONTINUE;
    }

    get_filename(filepath, filename_buffer);

    printf("\t%s -- %ld bytes\n", filename_buffer, stat_buffer->st_size);
    inc_size(stat_buffer->st_size);
    
    return CONTINUE;
}


int main(const int argc, const char** argv) {
    if (argc != 2) {
        puts("Wrong arguments");
        exit(EXIT_FAILURE);
    }

    total_size = 0;

    __ignore__ ftw(
        argv[1], 
        (int (*)(const char*, const struct stat*, int))entry_handle, 
        OPEN_LIMIT
    );

    if (errno)
        perror(argv[1]);
    else
        printf("\nTotal size: %lld bytes\n", total_size);

    return 0;
}
