#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>

#include"filenames.h"
#include"memory_blocks.h"
#include"errors.h"


error initialize(BlockList* block_list, size_t max_size) {
    block_list->memory_blocks = calloc(max_size, sizeof(void*));
    block_list->max_size = max_size;
    block_list->size = 0;

    return block_list->memory_blocks ? NO_ERROR : MEMORY_ERROR;
}


error get_index(BlockList* block_list, void** dest, size_t index) {
    if(block_list->size <= index) return INDEX_ERROR;
    if(!block_list->memory_blocks[index]) return NULLPTR_ERROR;
    if(!block_list->memory_blocks) return MEMORY_ERROR;

    *dest = block_list->memory_blocks[index];
    return NO_ERROR;
}


error delete_index(BlockList* block_list, size_t index) {
    if(block_list->size <= index) return INDEX_ERROR;
    if(!block_list->memory_blocks[index]) return NULLPTR_ERROR;
    if(!block_list->memory_blocks) return MEMORY_ERROR;

    free(block_list->memory_blocks[index]);
    block_list->memory_blocks[index] = NULL;
    return NO_ERROR;
}


void destroy_list(BlockList* block_list) {
    for(size_t i = 0; i < block_list->max_size; i++)
        if(block_list->memory_blocks[i])
            free(block_list->memory_blocks[i]);

    free(block_list->memory_blocks);
}


error load_file(BlockList* block_list, char* filepath) {
    char filename[FILENAME_MAX];
    char tmp_filename[FILENAME_MAX];
    char tmp_filepath[FILENAME_MAX + 20];

    if(block_list->size >= block_list->max_size) return LIST_OVERFLOW_ERROR;

    char* wc_call = malloc((FILENAME_MAX + strlen(filepath) + 60) * sizeof(char));
    char* rm_call = malloc((FILENAME_MAX + 32) * sizeof(char));

    get_filename(filepath, filename);
    set_tmp_filename(filename, tmp_filename);

    sprintf(tmp_filepath, "/tmp/%s", tmp_filename);
    sprintf(wc_call, "<%s wc -mwl >> %s", filepath, tmp_filepath);
    sprintf(rm_call, "rm %s", tmp_filepath);

    if(system(wc_call)) {
        free(wc_call);
        free(rm_call);
        return ARGUMENT_ERROR;
    }

    FILE* tmp_file;
    tmp_file = fopen(tmp_filepath, "r");
    
    if(!tmp_file) {
        free(wc_call);
        free(rm_call);
        return FILE_ERROR;
    }

    struct stat tmp_file_stat;
    stat(tmp_filepath, &tmp_file_stat);

    char* tmp_file_text = malloc(tmp_file_stat.st_size * sizeof(char));

    static size_t ignore = 0;
    if(getline(&tmp_file_text, &ignore, tmp_file) == -1) {
        free(wc_call);
        free(rm_call);
        return FILE_ERROR;
    }

    tmp_file_text[tmp_file_stat.st_size-1] = '\0';

    if(system(rm_call)) {
        free(wc_call);
        free(rm_call);
        return FILE_ERROR;
    }

    block_list->memory_blocks[block_list->size++] = (void*)tmp_file_text;
    
    free(wc_call);
    free(rm_call);

    return NO_ERROR;
}
