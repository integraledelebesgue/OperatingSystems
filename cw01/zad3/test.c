#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define BUFFER_LENGTH 1000


int get_file_count(char* buffer) {
    static const char file_count_stream_call[] = "ls -R ./ | egrep '\\.c$|\\.h$' | wc -l";
    FILE* file_count_stream = popen(file_count_stream_call, "r");
    size_t file_count = atoi(fgets(buffer, BUFFER_LENGTH, file_count_stream));
    fclose(file_count_stream);

    return file_count;
}


int main(const int argc, const char** argv) {
    static const char source_file_stream_call[] = "find -name *.[c,h]";
    char buffer[BUFFER_LENGTH];

    size_t file_count = get_file_count(buffer);

    FILE* file_stream = popen(source_file_stream_call, "r");
    FILE* repl_stream = popen(argv[1], "w");

    fprintf(repl_stream, "init %ld\n", file_count);
    
    while(fgets(buffer, BUFFER_LENGTH, file_stream)){
        buffer[strlen(buffer)-1] = '\0';
        //printf(".%s\n", buffer);
        fprintf(repl_stream, "count %s\n", buffer);
    }

    for(size_t i = 0; i < file_count; i++)
        fprintf(repl_stream, "delete %ld\n", i);    

    fprintf(repl_stream, "destroy\n");
    fprintf(repl_stream, "quit\n");
    
    pclose(file_stream);
    pclose(repl_stream);

    return 0;
}