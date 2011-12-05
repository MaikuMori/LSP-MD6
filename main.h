#pragma once
#define INPUT_BUFF_SIZE 64

typedef struct _MemoryBlock {
    int total_memory;
    int used_memory;
    int free_memory;
    struct _MemoryBlock * next;
    struct _MemoryBlock * prev;
} MemoryBlock;
