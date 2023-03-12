#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/stat.h>


typedef enum {NO_ERROR, ARGUMENT_ERROR} err;

#define HANDLE_STREAM(stream, stream_name)\
    if (!stream) {\
        perror(stream_name);\
        exit(ARGUMENT_ERROR);\
    }\


err list_dir(const char* dirpath) {
    struct dirent* curr_file;
    struct stat file_stat;
    long long total_size = 0;

    DIR* dir = opendir(dirpath);
    HANDLE_STREAM(dir, dirpath)

    while ((curr_file = readdir(dir))) {
        stat(curr_file->d_name, &file_stat);
        
        if (S_ISDIR(file_stat.st_mode))
            continue;

        total_size += file_stat.st_size;
        printf("%s -- %ld bytes\n", curr_file->d_name, file_stat.st_size);
    }

    printf("Total size: %lld bytes\n", total_size);
        
    closedir(dir);

    return NO_ERROR;
}


int main(const int argc, const char** argv) {
    if (argc != 2) {
        puts("Wrong arguments");
        exit(EXIT_FAILURE);
    }

    return list_dir(argv[1]);
}