#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#define NO_ERROR 0
#define ERROR -1

#define TEXT_MAX 4096


int main(const int argc, const char** argv) {
    FILE* input;
    FILE* output;
    char buffer[TEXT_MAX];

    input = popen("fortune", "r");
    output = popen("cowsay", "w");

    if (!(input && output))
        exit(EXIT_FAILURE);

    if (fread(buffer, sizeof(char), TEXT_MAX, input) == -1)
        exit(EXIT_FAILURE);

    if (fwrite(buffer, sizeof(char), strlen(buffer), output) == -1)
        exit(EXIT_FAILURE);

    pclose(input);
    pclose(output);

    return 0;
}