#include <stdio.h>
#include <stdlib.h>

//#include "bmp.h"
#include "algorithms.h"
#include "main.h"

MemoryBlock * mb_head = NULL;

//Allocate (simulate) free memory according to chunks file.
int allocate_memory(FILE *data) {
    MemoryBlock * mb_cur = NULL;
    char line[INPUT_BUFF_SIZE];
    int size;
    int chunks = 0;

    while(fgets(line, INPUT_BUFF_SIZE, data) != NULL) {
        size = atoi(line);

        //Either input was < 1 or it failed to convert to int, both are wrong.
        if (size < 1) {
            return 0;
        }

        //Allocate a new memory block.
        mb_cur = (MemoryBlock *) malloc(sizeof(MemoryBlock));
        mb_cur->free = 1;
        mb_cur->size = size;
        mb_cur->used = 0;

        if(mb_head) {
            mb_cur->prev = mb_head;
            mb_head->next = mb_cur;
        } else {
            mb_head = mb_cur;
        }

        chunks++;
    }

    return (chunks > 0) ? 1 : 0;
}

void help(void) {
    printf("Usage: md6\n");
    printf(" -c <filename>:\n");
    printf("\t Input file which defines free memory block sizes.\n");
    printf(" -s <filename>:\n");
    printf("\t Input file which contains memory requests\n");
    printf("Both arguments required.\n");
}

int main(int argc, char * argv[]) {
    int i, r;

    FILE * chunks_file;
    FILE * size_file;


    //Validate and parse input.
    if(argc != 5) {
        printf("Error: Wrong argument count!\n");
        help();
        return EXIT_FAILURE;
    }

    i = 1;
    while(i != 2 + 1) {
        switch(argv[i][1]) {
            case 'c':
                chunks_file = fopen(argv[2], "r");

                if(!chunks_file) {
                    printf("Error: Couldn't open chunks file!\n");
                    return EXIT_FAILURE;
                }
                i += 2;
                break;
            case 's':
                size_file = fopen(argv[2], "r");

                if(!size_file) {
                    printf("Error: Couldn't open size file!\n");
                    return EXIT_FAILURE;
                }
                i += 2;
                break;
            default:
                printf("Error: Wrong unknown argument.\n");
                help();
                return EXIT_FAILURE;
        }
    }

    //Simulate free memory according to chunks file.
    r = allocate_memory(chunks_file);
    if(!r) {
         printf("Error: Failed to parse chunks file, probably currupt file.\n");
         return EXIT_FAILURE;
    }

    //Close the files.
    fclose(chunks_file);
    fclose(size_file);

    return EXIT_SUCCESS;
}
