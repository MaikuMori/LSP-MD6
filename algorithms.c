#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "algorithms.h"

int alg_first_fit(MemoryBlock * memorylist_head, FILE * data) {
    MemoryBlock * mb_cur;
    char line[INPUT_BUFF_SIZE];
    int size, unallocated = 0;

    while(fgets(line, INPUT_BUFF_SIZE, data) != NULL) {
        size = atoi(line);
        //Either input was < 1 or it failed to convert to int, both are wrong.
        if (size < 1) {
            return -1;
        }

        for(mb_cur = memorylist_head; mb_cur != NULL; mb_cur = mb_cur->next) {
            if((mb_cur->size >= size) && (mb_cur->free == 1)) {
                mb_cur->free = 0;
                mb_cur->used = size;
                break;
            }
        }

        //Failed to find memory.
        if (mb_cur == NULL) {
            unallocated += size;
            continue;
        }
    }

    return unallocated;
}

int alg_next_fit(MemoryBlock * memorylist_head, FILE * data) {
    MemoryBlock * mb_cur, * mb_temp = NULL;
    char line[INPUT_BUFF_SIZE];
    int size, unallocated = 0;

    while(fgets(line, INPUT_BUFF_SIZE, data) != NULL) {
        size = atoi(line);
        //Either input was < 1 or it failed to convert to int, both are wrong.
        if (size < 1) {
            return -1;
        }

        for(mb_cur = memorylist_head; mb_cur != NULL; mb_cur = mb_cur->next) {
            if((mb_cur->size >= size) && (mb_cur->free == 1)) {
                if (mb_temp != NULL) {
                    mb_cur->free = 0;
                    mb_cur->used = size;
                    break;
                } else {
                    //We found the first one.
                    mb_temp = mb_cur;
                }
            }
        }

        //Failed to find memory.
        if (mb_cur == NULL) {
            unallocated += size;
            continue;
        }
    }
    return unallocated;
}

int alg_best_fit(MemoryBlock * memorylist_head, FILE * data) {
    MemoryBlock * mb_cur, * best_block_known_for_now;
    char line[INPUT_BUFF_SIZE];
    int size, smallest_known_size;

    while(fgets(line, sizeof line, data) != NULL) {
        size = atoi(line);

        //Either input was < 1 or it failed to convert to int, both are wrong.
        if (size < 1) {
            return 0;
        }

        // Reset smallest known block
        smallest_known_size = -1;
        best_block_known_for_now = NULL;

        for(mb_cur = memorylist_head; mb_cur != NULL; mb_cur = mb_cur->next) {

            // Perfect fit
            if(mb_cur->free == size) {
                best_block_known_for_now = mb_cur;
                break;
            }
            // first good known fit or a better fit
            else if((mb_cur->free > size && smallest_known_size == -1) ||
                (mb_cur->free > size && mb_cur->free < smallest_known_size)) {

                best_block_known_for_now = mb_cur;
                smallest_known_size = mb_cur->free;
            }
        }

        // allocate memory here
        // Memory is full
        if(best_block_known_for_now == NULL) {
            return -1;
        }
        else {
            best_block_known_for_now->free = 0;
            best_block_known_for_now->used = size;
        }
    }
    return 1;
}

int alg_worst_fit(MemoryBlock * memorylist_head, FILE * data) {
    MemoryBlock * mb_cur, * best_block_known_for_now;
    char line[INPUT_BUFF_SIZE];
    int size, largest_known_size;

    while(fgets(line, sizeof line, data) != NULL) {
        size = atoi(line);

        //Either input was < 1 or it failed to convert to int, both are wrong.
        if (size < 1) {
            return 0;
        }

        //Reset smallest known block.
        largest_known_size = 0;
        best_block_known_for_now = NULL;

        for(mb_cur = memorylist_head; mb_cur != NULL; mb_cur = mb_cur->next) {

            if((mb_cur->free > size && largest_known_size < mb_cur->free)) {

                best_block_known_for_now = mb_cur;
                largest_known_size = mb_cur->free;
            }
        }

        // allocate memory here
        // Memory is full
        if(best_block_known_for_now == NULL) {
            return -1;
        }
        else {
            best_block_known_for_now->free = 0;
            best_block_known_for_now->used = size;
        }
    }
    return 1;
}
