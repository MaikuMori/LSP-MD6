//Enable some of the new functions.
#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "algorithms.h"
#include "tests.h"
#include "main.h"

//How many allocation algorithms we have.
#define ALG_COUNT 4
//How many times to measure each algorithm.
#define MEASURE_COUNT 10000000
//Due to function being really fast we have to measure it multiple times and get
//mean.
#define MEASURE_COUNT_INNER 1
//Path to tests
#define TEST_PATH "./tests"

//Head of memory block linked list.
MemoryBlock * mb_head = NULL;
//Sizes file content.
int * requests;
unsigned int requests_total = 0;
unsigned int requests_size = 20;
//Algorithm function lookup table.
int (* alg_functions[ALG_COUNT])(MemoryBlock * , int *, int);

//Allocate (simulate) free memory according to chunks file.
int memory_allocate(FILE *data) {
    MemoryBlock * mb_temp, * mb_cur = NULL;
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
        mb_temp = (MemoryBlock *) malloc(sizeof(MemoryBlock));
        mb_temp->total_memory = size;
        mb_temp->free_memory = size;
        mb_temp->next = NULL;

        if(mb_cur) {
            mb_cur->next = mb_temp;
        }

        mb_cur = mb_temp;

        if(!mb_head) {
            mb_head = mb_cur;
        }

        chunks++;
    }

    return (chunks > 0) ? 1 : 0;
}

//Read requests.
int requests_read(FILE * sizes_file) {
    char line[INPUT_BUFF_SIZE];
    int size;

    requests = (int *) malloc(sizeof(int) * requests_size);

    while(fgets(line, INPUT_BUFF_SIZE, sizes_file) != NULL) {
        size = atoi(line);
        //Either input was < 1 or it failed to convert to int, both are wrong.
        if(size < 1) {
            return 0;
        }

        if (requests_total == requests_size) {
            requests_size *= 2;
            requests = (int *) realloc(requests, requests_size);
        }

        requests[requests_total] = size;
    }

    return 1;
}

//Resets all memory chunks back to 'free'.
void memory_reset(void) {
    MemoryBlock * mb_cur = NULL;

    for(mb_cur = mb_head; mb_cur != NULL; mb_cur = mb_cur->next) {
        mb_cur->free_memory = mb_cur->total_memory;
    }
}

//Get fragmentation information from memory.
float get_fragmentation(void) {
    MemoryBlock * mb_cur;
    float total = 0, largest = 0, free = 0;

    for(mb_cur = mb_head; mb_cur != NULL; mb_cur = mb_cur->next) {
        free = (float) mb_cur->free_memory;
        total += free;

        if(free > largest) {
            largest = free;
        }
    }

    return (1.0 - (largest / total));
}

//Calculate difference between two timespec structs.
struct timespec time_diff(struct timespec start, struct timespec end) {
    struct timespec temp;
    if ((end.tv_nsec - start.tv_nsec) < 0) {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}

//Do the measurements for each algorithm.
void measure(FILE* chunks_file, FILE * requests_file)
{
    //Benchmark times.
    struct timespec start_time_proc, end_time_proc, sum_proc;
    //Fragmentation info.
    float frag;
    //Temp MemoryBlock.
    MemoryBlock * mb_temp;

    int i, j, k, r;
    //Simulate free memory according to chunks file.
    r = memory_allocate(chunks_file);

    if(!r) {
         printf("Error: Failed to parse chunks file"
                " probably corrupt file.\n");
         return;
    }

    r = requests_read(requests_file);

    if(!r) {
         printf("Error: Failed to parse size file"
                " probably corrupt file.\n");
         return;
    }

    //For each algorithm test it's running time.
    for(i = 0; i < ALG_COUNT; i++) {
        memset(&sum_proc, 0, sizeof(struct timespec));

        printf("Running algorithm %d.\n", i + 1);
        for(j = 0; j < MEASURE_COUNT; j++) {
            //Reset the memory to unused.
            memory_reset();

            //Get start time.
            clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time_proc);

            //Call the algorithm.
            for(k = 0; k < MEASURE_COUNT_INNER; k++)
                r = alg_functions[i](mb_head, requests, requests_total);

            //Get end time and calculate difference.
            clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_time_proc);
            end_time_proc = time_diff(start_time_proc, end_time_proc);

            //Sum with just in case overflow check, but there's almost no
            //chance that it will reach number that high there fore secs
            //are not used later.
            sum_proc.tv_sec += end_time_proc.tv_sec;
            sum_proc.tv_nsec += end_time_proc.tv_nsec;
            if (sum_proc.tv_nsec > 1000000000) {
                sum_proc.tv_nsec -= 1000000000;
                sum_proc.tv_sec++;
            }
        }

        //Get memory fragmentation.
        frag = get_fragmentation();

        //Check response from algorithm.
        if(r < 0)
            printf("Algorithm failed to open size file.\n");

        //Print the benchmark.
        printf("CPU-time: %.2lfs %.2lfns, fragmentation: %2.2f%%, unallocated: %d\n\n",
               (double) sum_proc.tv_sec  / MEASURE_COUNT,
               (double) sum_proc.tv_nsec  / MEASURE_COUNT, frag * 100, r);
    }

    //Free chunks.
    while(mb_head) {
        mb_temp = mb_head->next;
        free(mb_head);
        mb_head = mb_temp;
    }

    //Free requests.
    free(requests);
    requests_total = 0;
    requests_size = 20;
}

//Runs measurements tests.
void run_tests(TestNode * test)
{
    FILE * chunks_file;
    FILE * requests_file;
    while(test) {
        //Skip if we don't have both files.
        if((test->chunk_path == NULL) || (test->request_path == NULL)) {
            printf("Skipping test '%s', doesn't have both input files.\n",
                    test->name);
            test = test->next;
            continue;
        }

        chunks_file = fopen(test->chunk_path, "r");
        requests_file = fopen(test->request_path, "r");

        //Skip if we can't open either of the files.
        if(!chunks_file) {
            test = test->next;
            printf("Skipping test '%s', can't open chunks file.\n",
                    test->name);
            continue;
        }
        if(!requests_file) {
            test = test->next;
            printf("Skipping test '%s', can't open requests file.\n",
                    test->name);
            continue;
        }

        printf("Doing measurements for test '%s' ...\n", test->name);
        measure(chunks_file, requests_file);

        //Close the files.
        fclose(chunks_file);
        fclose(requests_file);

        test = test->next;
        printf("\n");
    }
    printf("Done!\n");
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
    //File pointers.
    FILE * chunks_file;
    FILE * requests_file;
    //Head of test list.
    TestNode * test_head;

    int i;
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
                        printf("Error: Could not open chunks file!\n");
                        return EXIT_FAILURE;
                    }
                    i += 2;
                    break;
                case 's':
                    requests_file = fopen(argv[i + 1], "r");
                    if(!requests_file) {
                        printf("Error: Could not open requests file!\n");
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
        //Do the measurements for input files.
        measure(chunks_file, requests_file);

        //Close the files.
        fclose(chunks_file);
        fclose(requests_file);
    } else {
        //Do all tests and then free them.
        test_head = tests_get(TEST_PATH);
        run_tests(test_head);
        tests_free(test_head);
    }

    return EXIT_SUCCESS;
}
