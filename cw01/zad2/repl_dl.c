#include<stdio.h>
#include<time.h>
#include<sys/times.h>
#include<dlfcn.h>

#include"../zad1/errors.h"
#include"repl.h"
#include"command_processing.h"

#define DYNAMIC_LOAD_ERROR(symbol_name) {\
    puts("Failed to dynamically load " #symbol_name " -- terminated");\
    exit(1);\
}\


error (* get_index_fptr)(BlockList*, void**, size_t);
error (* initialize_fptr)(BlockList*, size_t);
error (* delete_index_fptr)(BlockList*, size_t);
error (* load_file_fptr)(BlockList*, char*);
error (* destroy_list_fptr)(BlockList*);


error show_block(Session* session) {
    void* block_view;
    error error;

    if((error = (*get_index_fptr)(session->block_list, &block_view, session->arg.numeric)))
        return error;
    
    printf("%s\n", (char*)block_view);
    return NO_ERROR;    
}


error handle(error error) {
    switch(error) {
        case NO_ERROR:
            break;

        case MEMORY_ERROR:
            puts("Runtime memory management error -- terminated");
            return BREAK;

        case NULLPTR_ERROR:
            puts("Nullptr excepion - attempted to access non-existing element");
            break;

        case INDEX_ERROR:
            puts("Index exception - index out of range");
            break;

        case FILE_ERROR:
            puts("Runtime file I/O error -- terminated");
            return BREAK;

        case ARGUMENT_ERROR:
            puts("Wrong argument exception");
            break;

        case IO_ERROR:
            puts("Runtime I/O error -- terminated");
            return BREAK;

        case COMMAND_ERROR:
            puts("Unknown command exception");
            break;

        case LIST_OVERFLOW_ERROR:
            puts("Overflow exception - list is full");
            break;

        case LIST_UNINITIALIZED_ERROR:
            puts("Element uninitialized exception - attempted to access uninitialized object");
            break;

        case BREAK:
            return BREAK;
    };

    return NO_ERROR;
}


error process_state(Session* session) {
    error err;

    switch(session->state) {
        case READ:
            printf("input $ ");
            if((err = read_command(session))) return err;
            return process_command(session);

        case INIT:
            printf("Initializing new list of %ld blocks...\n", session->arg.numeric);
            session->state = READ;

            if((err = (*initialize_fptr)(session->block_list, session->arg.numeric)))
                return err;

            session->initialized = 1;
            return NO_ERROR;

        case SHOW:
            session->state = READ;

            if(!session->block_list->memory_blocks) return LIST_UNINITIALIZED_ERROR;

            printf("Block at index %ld:\n", session->arg.numeric);
            return show_block(session);

        case DELETE:
            session->state = READ;
            
            if(!session->block_list->memory_blocks) return LIST_UNINITIALIZED_ERROR;
            
            printf("Deleting block at index %ld...\n", session->arg.numeric);
            return (*delete_index_fptr)(session->block_list, session->arg.numeric);

        case DESTROY:
            session->state = READ;

            if(!session->initialized) return LIST_UNINITIALIZED_ERROR;
            if(!session->block_list->memory_blocks) return MEMORY_ERROR;
            
            printf("Destroying current list...\n");
            (*destroy_list_fptr)(session->block_list);
            session->initialized = 0;
            return NO_ERROR;

        case COUNT:
            session->state = READ;

            if(!session->initialized) return LIST_UNINITIALIZED_ERROR;

            printf("Attempting to read file %s...\n", session->arg.string);
            return (*load_file_fptr)(session->block_list, session->arg.string);

        case NOTHING:
            puts("Unknown command");
            session->state = READ;
            return NO_ERROR;

        case QUIT:
            if(session->initialized) (*destroy_list_fptr)(session->block_list);
            return BREAK;
    }

    return NO_ERROR;
}


void repl(Session* session) {
    struct tms cpu_time_buffer;
    struct timespec real_time_buffer;

    register time_t real_time;
    register time_t user_cpu_time;
    register time_t system_cpu_time;

    while(1) {
        timespec_get(&real_time_buffer, TIME_UTC);
        times(&cpu_time_buffer);

        real_time = real_time_buffer.tv_nsec;
        user_cpu_time = cpu_time_buffer.tms_cutime;
        system_cpu_time = cpu_time_buffer.tms_cstime;

        if(handle(process_state(session)) == BREAK)
            return;

        timespec_get(&real_time_buffer, TIME_UTC);
        times(&cpu_time_buffer);

        if(session->state == READ) printf(
            "Executed in:\n\treal: %ld ns\n\tuser cpu: %ld ns\n\tsystem cpu: %ld ns\n",
            real_time_buffer.tv_nsec - real_time,
            cpu_time_buffer.tms_cutime - user_cpu_time,
            cpu_time_buffer.tms_cstime - system_cpu_time
        );
    }

}

void load_functions(void* handle) {
    get_index_fptr = (error (*)(BlockList*, void**, size_t))dlsym(handle, "get_index");
    if(!get_index_fptr) DYNAMIC_LOAD_ERROR(function "get_index")

    initialize_fptr = (error (*)(BlockList*, size_t))dlsym(handle, "initialize");
    if(!initialize_fptr) DYNAMIC_LOAD_ERROR(function "initialize")

    delete_index_fptr = (error (*)(BlockList*, size_t))dlsym(handle, "delete_index");
    if(!delete_index_fptr) DYNAMIC_LOAD_ERROR(function "delete_index")

    load_file_fptr = (error (*)(BlockList*, char*))dlsym(handle, "load_file");
    if(!load_file_fptr) DYNAMIC_LOAD_ERROR(function "load_file")

    destroy_list_fptr = (error (*)(BlockList*))dlsym(handle, "destroy_list");
    if(!destroy_list_fptr) DYNAMIC_LOAD_ERROR(function "destroy_list")
}


int main(const int argc, const char** argv) {

    void* handle = dlopen("lib_zad1.so.1", RTLD_LAZY);
    if(!handle) DYNAMIC_LOAD_ERROR(library "lib_zad1.so.1")

    load_functions(handle);

    BlockList block_list;
    Session session;

    session.block_list = &block_list;
    session.state = READ;
    session.initialized = 0;

    repl(&session);

    dlclose(handle);

    return 0;
}
