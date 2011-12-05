//Enable some of the new functions.
#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "algorithms.h"
#include "main.h"

#define ALG_COUNT 0
#define MEASURE_COUNT 0

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
        mb_cur->prev = NULL;
        mb_cur->next = NULL;

        if(mb_head) {
            mb_cur->prev = mb_head;
            mb_head->next = mb_cur;
        } else {
            mb_head = mb_cur;
        }
        printf("%p %p\n", mb_head, mb_cur);
        chunks++;
    }

    return (chunks > 0) ? 1 : 0;
}

void reset_memory(void) {
    MemoryBlock * mb_cur = NULL;

    for(mb_cur = mb_head; mb_cur->next != NULL; mb_cur = mb_head->next) {
        mb_cur->free = 1;
        mb_cur->used = 0;
    }
}

//Get fragmention information from memory.
float get_fragmentation(void) {
    MemoryBlock * mb_cur;
    float total = 0, largest = 0, free = 0;

    for(mb_cur = mb_head; mb_cur != NULL; mb_cur = mb_cur->next) {
        free = (float) mb_cur->free;
        total += free;

        if (free > largest ){
            largest = free;
        }
    }
    printf("%f %f %f\n",total, largest, free);
    return (1.0 - (largest / total));
}

void print_memory(void) {
    MemoryBlock * mb_cur = NULL;
    for(mb_cur = mb_head; mb_cur != NULL; mb_cur = mb_cur->next){
        printf("Memory buffer size - %i, used - %i\n", mb_cur->size, mb_cur->used);
    }
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
    //Avg time.
    unsigned long int sum_rt, sum_proc;
    //Fragmentation info.
    float frag;
    //File pointers.
    FILE * chunks_file;
    FILE * size_file;
    //Function lookup table.
    int (* alg_functions[ALG_COUNT])(MemoryBlock * , FILE * );

    int i, j, r;
    unsigned char do_tests = 0;

    //Fill the function lookup table.
    alg_functions[0] = alg_first_fit;
    alg_functions[1] = alg_next_fit;
    alg_functions[2] = alg_best_fit;
    alg_functions[3] = alg_worst_fit;

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
        print_memory();

        if(!r) {
             printf("Error: Failed to parse chunks file"
                    " probably currupt file.\n");
             return EXIT_FAILURE;
        }

        //For each algorithm test it's runing time.
        for(i = 0; i < ALG_COUNT; i++) {
            sum_rt = sum_proc = 0;
            printf("Running algorithm %d.\n", i + 1);
            for(j = 0; j < MEASURE_COUNT; j++) {
                //Reset the memory to unused.
                reset_memory();
                rewind(size_file);

                //Get 'realtime' clock, sadly also measures the next function,
                //but that shouldn't matter here as for real measurements
                //we're using CPU time.
                clock_gettime(CLOCK_MONOTONIC, &start_time_rt);
                clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time_proc);

                //Call the algorithm.
                r = alg_functions[i](mb_head, size_file);

                //Get timer readings.
                clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_time_proc);
                clock_gettime(CLOCK_MONOTONIC, &end_time_rt);
            }

            sum_rt += (long) (end_time_rt.tv_nsec - start_time_rt.tv_nsec);
            sum_proc += (long) (end_time_proc.tv_nsec -
                                    start_time_proc.tv_nsec);

            frag = get_fragmentation();

            //Check response from algorithm.
            if(r < 0)
                printf("Algorithm failed to open size file.");

            //Print the benchmark.
            printf("Realtime: %lf, CPU: %lf, fragmentation: %f, "
                   "unallocated: %d\n\n",
                   (double) sum_rt / MEASURE_COUNT,
                   (double) sum_proc  / MEASURE_COUNT,
                   frag, r);
        }

        //Close the files.
        fclose(chunks_file);
        fclose(size_file);
    } else {
        //Do all tests.
    }

    return EXIT_SUCCESS;
}
