#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<error.h>

typedef enum {NO_ERROR, ARGUMENT_ERROR} err;

#define __ignore__ (void)

#define HANDLE_DESCRIPTOR(desc, filename)\
    if (desc == -1) {\
        perror(filename);\
        return ARGUMENT_ERROR;\
    }\

#define HANDLE_STREAM(stream, filename)\
    if (!stream) {\
        perror(filename);\
        return ARGUMENT_ERROR;\
    }\


#ifdef SYS
err replace_chars(const char to_replace, const char replace_by, const char* source_filename, const char* dest_filename) {
    int source_descriptor = open(source_filename, O_RDONLY);
    int dest_descriptor = creat(dest_filename, O_CREAT);
    char buffer;

    HANDLE_DESCRIPTOR(source_descriptor, source_filename)
    HANDLE_DESCRIPTOR(dest_descriptor, dest_filename)

    while (read(source_descriptor, (void*)&buffer, 1) > 0)
        __ignore__ write(
            dest_descriptor, 
            buffer == to_replace ? (void*)&replace_by : (void*)&buffer,
            1
        );

    close(source_descriptor);
    close(dest_descriptor);

    return NO_ERROR;
}
#else
err replace_chars(const char to_replace, const char replace_by, const char* source_filename, const char* dest_filename) {
    FILE* source = fopen(source_filename, "r");
    FILE* dest = fopen(dest_filename, "w+");
    char buffer;

    HANDLE_STREAM(source, source_filename);
    HANDLE_STREAM(dest, dest_filename);

    while (fread((void*)&buffer, sizeof(char), 1, source))
        __ignore__ fwrite(
            buffer == to_replace ? (void*)&replace_by : (void*)&buffer, 
            sizeof(char),
            1,
            dest
        );

    fclose(source);
    fclose(dest);

    return NO_ERROR;
}
#endif


int main(const int argc, const char** argv) {
    if (argc != 5) {
        printf("Wrong arguments\n");
        exit(EXIT_FAILURE);
    }

    return replace_chars(*argv[1], *argv[2], argv[3], argv[4]);   
}
