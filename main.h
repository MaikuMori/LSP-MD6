#pragma once

//Line input buffer.
#define INPUT_BUFF_SIZE 16

//All memory chucks are stored in linked list.
//'MemoryBlock' is the node of this linked list.
typedef struct _MemoryBlock {
    int total_memory;
    int free_memory;
    struct _MemoryBlock * next;
} MemoryBlock;
