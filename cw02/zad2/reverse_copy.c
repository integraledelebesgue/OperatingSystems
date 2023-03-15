#include<stdio.h>
#include<stdlib.h>
#include<error.h>
#include<sys/stat.h>


typedef enum {NO_ERROR, ARGUMENT_ERROR} err;

#ifndef BUFFER_SIZE 
#define BUFFER_SIZE 1
#endif

#define HANDLE_STREAM(stream, filename)\
    if (!stream) {\
        perror(filename);\
        return ARGUMENT_ERROR;\
    }\


#define COPY_REVERSED_BLOCK(_source, _dest, _curr_size, _buffer, _i, _read_size) {\
    curr_size = fread((void*)_buffer, 1, _read_size, _source);\
    \
    for (_i = _curr_size; _i-- > 0;)\
        fwrite((void*)&_buffer[i], 1, 1, _dest);\
    \
    fseek(_source, -2 * BUFFER_SIZE, SEEK_CUR);\
}\


err reverse_copy(const char* source_filename, const char* dest_filename) {
    FILE* source = fopen(source_filename, "r");
    FILE* dest = fopen(dest_filename, "w+");
    char buffer[BUFFER_SIZE];
    size_t i;
    size_t curr_size;
    size_t source_end;
    struct stat source_stat;

    HANDLE_STREAM(source, source_filename)
    HANDLE_STREAM(dest, dest_filename)

    stat(source_filename, &source_stat);
    source_end = source_stat.st_size % BUFFER_SIZE;

    fseek(source, -BUFFER_SIZE, SEEK_END);

    while (ftell(source) >= BUFFER_SIZE)
        COPY_REVERSED_BLOCK(source, dest, curr_size, buffer, i, BUFFER_SIZE)

    COPY_REVERSED_BLOCK(source, dest, curr_size, buffer, i, BUFFER_SIZE)

    if (BUFFER_SIZE < source_stat.st_size) {
        fseek(source, 0, 0);
        COPY_REVERSED_BLOCK(source, dest, curr_size, buffer, i, source_end)
    }

    fwrite((void*)"\n", sizeof(char), 1, dest);
    
    fclose(source);
    fclose(dest);

    return NO_ERROR;
}


int main(const int argc, const char** argv) {
    if(argc != 3) {
        puts("Wrong arguments");
        exit(EXIT_FAILURE);
    }

    return reverse_copy(argv[1], argv[2]);
}
