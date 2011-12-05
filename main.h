#pragma once
#define INPUT_BUFF_SIZE 64

typedef struct _MemoryBlock {
    int size;
    int free;
    int used;
    struct _MemoryBlock * next;
    struct _MemoryBlock * prev;
} MemoryBlock;


