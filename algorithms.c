#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "algorithms.h"

int alg_first_fit(MemoryBlock * memorylist_head, int * requests, int total) {
    MemoryBlock * mb_cur;
    int i, size, unallocated = 0;

    for(i = 0; i < total; i++) {
        size = requests[i];

        for(mb_cur = memorylist_head; mb_cur != NULL; mb_cur = mb_cur->next) {
            if(mb_cur->free_memory >= size) {
                mb_cur->free_memory -= size;
                break;
            }
        }

        //Failed to find memory.
        if(mb_cur == NULL) {
            unallocated += size;
        }
    }

    return unallocated;
}

int alg_next_fit(MemoryBlock * memorylist_head, int * requests, int total) {
    MemoryBlock * mb_cur;
    MemoryBlock * mb_last_found = memorylist_head;

    int i, size, unallocated = 0;

    for(i = 0; i < total; i++) {
        size = requests[i];

        for(mb_cur = mb_last_found; ; mb_cur = mb_cur->next) {

            if (mb_cur == NULL) {
                mb_cur = memorylist_head;
            }
            //Failed to find memory.
            if (mb_cur == mb_last_found){
                 unallocated += size;
                 break;
            }

            if(mb_cur->free_memory >= size) {
                mb_cur->free_memory -= size;
                break;
            }
        }
    }
    return unallocated;
}

int alg_best_fit(MemoryBlock * memorylist_head, int * requests, int total) {
    MemoryBlock * mb_cur, * best_block;
    int i, size, smallest_known_size, unallocated = 0;

    for(i = 0; i < total; i++) {
        size = requests[i];

        //Reset smallest known block.
        smallest_known_size = -1;
        best_block = NULL;

        for(mb_cur = memorylist_head; mb_cur != NULL; mb_cur = mb_cur->next) {
            //Perfect fit.
            if(mb_cur->free_memory == size) {
                best_block = mb_cur;
                break;
            } else if((mb_cur->free_memory > size) &&
                        ((smallest_known_size == -1) ||
                         (mb_cur->free_memory < smallest_known_size))) {
                //First good known fit or a better fit.
                best_block = mb_cur;
                smallest_known_size = mb_cur->free_memory;
            }
        }

        if(best_block == NULL) {
            unallocated += size;
        } else {
            best_block->free_memory -= size;
        }
    }
    return unallocated;
}

int alg_worst_fit(MemoryBlock * memorylist_head, int * requests, int total) {
    MemoryBlock * mb_cur, * best_block;
    int i, size, largest_known_size, unallocated = 0;

    for(i = 0; i < total; i++) {
        size = requests[i];

        //Reset smallest known block.
        largest_known_size = 0;
        best_block = NULL;

        for(mb_cur = memorylist_head; mb_cur != NULL; mb_cur = mb_cur->next) {

            if((mb_cur->free_memory > size) &&
              (largest_known_size < mb_cur->free_memory)) {

                best_block = mb_cur;
                largest_known_size = mb_cur->free_memory;
            }
        }

        if(best_block == NULL) {
            unallocated += size;
        } else {
            best_block->free_memory -= size;
        }
    }
    return unallocated;
}
