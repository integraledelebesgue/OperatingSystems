#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<dirent.h>
#include<string.h>
#include<wait.h>


typedef enum {NO_ERROR, FILE_ERROR, PROCESS_ERROR} err;


#define MAX_PATTERN_LENGTH 256
static size_t pattern_length;
static char pattern[MAX_PATTERN_LENGTH];


#define SAFE_FORK(child_pid)\
    child_pid = fork();\
    if (child_pid == -1) {\
        fprintf(stderr, "PID %d", getpid());\
        perror("");\
        exit(PROCESS_ERROR);\
    }\


#define OPEN_FILE(path, stream)\
    stream = fopen(path, "r");\
    if (!stream) {\
        perror(path);\
        exit(FILE_ERROR);\
    }\


#define CLOSE_FILE(stream)\
    if(fclose(stream) == -1) {\
        perror("");\
        exit(FILE_ERROR);\
    }\


#define OPEN_DIR(path, stream)\
    stream = opendir(path);\
    if (!stream) {\
        perror(path);\
        exit(FILE_ERROR);\
    }\


#define CHANGE_DIR(path)\
    if(chdir(path)) {\
        perror(path);\
        exit(EXIT_FAILURE);\
    }\


#define CLOSE_DIR(stream)\
    if (closedir(stream) == -1) {\
        perror("");\
        exit(FILE_ERROR);\
    }\


#define GET_STAT(path, buffer)\
    if (stat(path, &buffer) == -1) {\
        perror(path);\
        exit(FILE_ERROR);\
    }\


void search_file(const char* filepath) {
    FILE* file_stream;
    char file_buffer[pattern_length];
    size_t read_size;

    OPEN_FILE(filepath, file_stream)

    read_size = fread((void*)file_buffer, sizeof(char), pattern_length, file_stream);

    if (read_size < pattern_length)
        goto exit;

    if(!strcmp(pattern, file_buffer)) {
        printf("String found in %s\n", filepath);
        fflush(stdout);
    }

    exit:
    CLOSE_FILE(file_stream)
}


void search(const char* filepath) {
    DIR* directory_stream;
    struct dirent* current_file;
    struct stat stat_buffer;
    pid_t child_pid;
    char path[PATH_MAX];

    realpath(filepath, path);
    
    GET_STAT(path, stat_buffer)

    if (S_ISREG(stat_buffer.st_mode)) {
        search_file(path);
        exit(EXIT_SUCCESS);
    }

    if (S_ISDIR(stat_buffer.st_mode)) {
        CHANGE_DIR(path);
        OPEN_DIR(path, directory_stream)

        while((current_file = readdir(directory_stream))) {
            SAFE_FORK(child_pid)

            if (child_pid != 0)
                continue;
            
            if (strcmp("..", current_file->d_name) && strcmp(".", current_file->d_name))
                search(current_file->d_name);

            exit(EXIT_SUCCESS);
        }

        CLOSE_DIR(directory_stream);

        #ifdef SYNCHRONIZE
        while(wait(NULL) > 0);
        #endif
    }
}


int main(const int argc, const char** argv) {
    if (argc != 3) {
        puts("Wrong arguments");
        exit(EXIT_FAILURE);
    }

    strcpy(pattern, argv[2]);
    pattern_length = strlen(argv[2]);

    search(argv[1]);

    exit(EXIT_SUCCESS);
}