//Enable some of the new functions.
#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#include "bmp.h"
#include "algorithms.h"
#include "main.h"

#define ALG_COUNT 2

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
        mb_cur->next = NULL;

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
    //Timers.
    struct timespec start_time_rt, start_time_proc;
    struct timespec end_time_rt, end_time_proc;
    //File pointers
    FILE * chunks_file;
    FILE * size_file;
    //Function lookup table.
    int (* alg_functions[ALG_COUNT])(MemoryBlock * , FILE * );

    int i, r;
    unsigned char do_tests = 0;

    //Fill the function lookup table.
    alg_functions[0] = alg_first_fit;
    alg_functions[1] = alg_next_fit;

    //Validate and parse input.
    if(argc == 5) {
        //If we got all arguments, we know what to do.
        i = 1;
        while(i < 5) {
            switch(argv[i][1]) {
                case 'c':
                    chunks_file = fopen(argv[i + 1], "r");

                    if(!chunks_file) {
                        printf("Error: Couldn't open chunks file!\n");
                        return EXIT_FAILURE;
                    }
                    i += 2;
                    break;
                case 's':
                    size_file = fopen(argv[i + 1], "r");

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
    } else if(argc == 1) {
        //No input, unleash the tests!
        do_tests = 1;
    } else {
        //Something is wrong.
        printf("Error: Wrong argument count!\n");
        help();
        return EXIT_FAILURE;
    }

    //Just working with 2 input files.
    if(!do_tests) {
        //Simulate free memory according to chunks file.
        r = allocate_memory(chunks_file);
        if(!r) {
             printf("Error: Failed to parse chunks file"
                    " probably currupt file.\n");
             return EXIT_FAILURE;
        }

        //For each algorithm test it's runing time.
        for(i = 0; i < ALG_COUNT; i++) {
            printf("Running algorithm %d.\n", i + 1);
            //Get 'realtime' clock, sadly also measures the next function,
            //but that shouldn't matter here as for real measurements
            //we're using CPU time.
            clock_gettime(CLOCK_MONOTONIC, &start_time_rt);
            clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time_proc);

            r = alg_functions[i](mb_head, size_file);

            //Get timer readings.
            clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_time_proc);
            clock_gettime(CLOCK_MONOTONIC, &end_time_rt);

            //Check response from algorithm.
            switch(r) {
                case 0:
                    printf("Algorithm couldn't read input file.\n");
                    break;
                case -1:
                    printf("Algorithm reported out of memory error.\n");
                    break;
            }

            //Print the benchmark.
            printf("Realtime: %ld %ld, CPU: %ld %ld\n\n",
                (long) (end_time_rt.tv_sec - start_time_rt.tv_sec),
                (long) (end_time_rt.tv_nsec - start_time_rt.tv_nsec),
                (long) (end_time_proc.tv_sec - start_time_proc.tv_sec),
                (long) (end_time_proc.tv_nsec - start_time_proc.tv_nsec)
            );
        }

        //Close the files.
        fclose(chunks_file);
        fclose(size_file);
    } else {
        //Do all tests.
    }

    return EXIT_SUCCESS;
}
