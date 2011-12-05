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
