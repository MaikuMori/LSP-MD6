#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "algorithms.h"

int alg_first_fit(MemoryBlock * memorylist_head, FILE * data) {
    MemoryBlock * mb_cur;
    char line[INPUT_BUFF_SIZE];
    int size;

    while(fgets(line, INPUT_BUFF_SIZE, data) != NULL) {
        size = atoi(line);
        //Either input was < 1 or it failed to convert to int, both are wrong.
        if (size < 1) {
            return 0;
        }

        for(mb_cur = memorylist_head; mb_cur != NULL; mb_cur = mb_cur->next) {
            if((mb_cur->size >= size) && (mb_cur->free == 1)) {
                mb_cur->free = 0;
                mb_cur->used = size;
                break;
            } else if(mb_cur->next == NULL) {
                //Not enough memory.
                return -1;
            }
        }

    }

    return 1;
}

int alg_next_fit(MemoryBlock * memorylist_head, FILE * data) {
    MemoryBlock * mb_cur, * mb_temp;
    char line[INPUT_BUFF_SIZE];
    int size;

    while(fgets(line, INPUT_BUFF_SIZE, data) != NULL) {
        size = atoi(line);
        //Either input was < 1 or it failed to convert to int, both are wrong.
        if (size < 1) {
            return 0;
        }

        for(mb_cur = memorylist_head; mb_cur != NULL; mb_cur = mb_cur->next) {
            if((mb_cur->size >= size) && (mb_cur->free == 1)) {
                for(mb_temp = mb_cur->next; mb_temp != NULL;
                                                    mb_temp = mb_temp->next) {

                    if((mb_temp->size >= size) && (mb_temp->free == 1)) {
                        mb_temp->free = 0;
                        mb_temp->used = size;
                        break;
                    } else if(mb_temp->next == NULL) {
                        //Not enough memory.
                        return -1;
                    }
                }
                break;
            } else if(mb_cur->next == NULL) {
                //Not enough memory.
                return -1;
            }
        }
    }
    return 1;
}

/**
 * Best fit
 * @param memorylist_head
 * @param data
 * @return 
 */
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
