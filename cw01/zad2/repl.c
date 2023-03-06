#include<stdio.h>
#include<time.h>
#include<sys/resource.h>

#include"../zad1/memory_blocks.h"
#include"../zad1/errors.h"
#include"repl.h"
#include"command_processing.h"


error show_block(Session* session) {
    void* block_view;
    error error;

    if((error = get_index(session->block_list, &block_view, session->arg.numeric)))
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
            puts("Nullptr exception - attempted to access non-existing element");
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

            if((err = initialize(session->block_list, session->arg.numeric)))
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
            return delete_index(session->block_list, session->arg.numeric);

        case DESTROY:
            session->state = READ;

            if(!session->initialized) return LIST_UNINITIALIZED_ERROR;
            if(!session->block_list->memory_blocks) return MEMORY_ERROR;
            
            printf("Destroying current list...\n");
            destroy_list(session->block_list);
            session->initialized = 0;
            return NO_ERROR;

        case COUNT:
            session->state = READ;

            if(!session->initialized) return LIST_UNINITIALIZED_ERROR;

            printf("Attempting to read file %s...\n", session->arg.string);
            return load_file(session->block_list, session->arg.string);

        case NOTHING:
            puts("Unknown command");
            session->state = READ;
            return NO_ERROR;

        case QUIT:
            if(session->initialized) destroy_list(session->block_list);
            return BREAK;
    }

    return NO_ERROR;
}


void repl(Session* session) {
    struct timespec real_time_usage;
    struct rusage cpu_usage;

    register time_t real_time;
    register time_t user_cpu_time;
    register time_t system_cpu_time;

    while(1) {
        timespec_get(&real_time_usage, TIME_UTC);
        getrusage(RUSAGE_CHILDREN, &cpu_usage);

        real_time = real_time_usage.tv_nsec;
        user_cpu_time = cpu_usage.ru_utime.tv_usec;
        system_cpu_time = cpu_usage.ru_stime.tv_usec;

        if(handle(process_state(session)) == BREAK)
            return;

        timespec_get(&real_time_usage, TIME_UTC);
        getrusage(RUSAGE_CHILDREN, &cpu_usage);

        if(session->state == READ) printf(
            "Executed in: Real time: %ld ns\tUser CPU: %ld us\tSystem CPU: %ld us\n",
            real_time_usage.tv_nsec - real_time,
            cpu_usage.ru_utime.tv_usec - user_cpu_time,
            cpu_usage.ru_stime.tv_usec - system_cpu_time
        );
    }

}


int main(const int argc, const char** argv) {

    BlockList block_list;
    Session session;

    session.block_list = &block_list;
    session.state = READ;
    session.initialized = 0;

    repl(&session);

    return 0;
}