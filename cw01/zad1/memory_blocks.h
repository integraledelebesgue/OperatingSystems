#ifndef MEMORY_BLOCKS
#define MEMORY_BLOCKS

#include<stdlib.h>
#include"errors.h"

typedef struct {
    void** memory_blocks;
    size_t max_size;
    size_t size;
} BlockList;

error initialize(BlockList* block_list, size_t max_size);
error get_index(BlockList* block_list, void** dest, size_t index);
error delete_index(BlockList* block_list, size_t index);
void destroy_list(BlockList* block_list);
error load_file(BlockList* block_list, char* filepath);

#endif
