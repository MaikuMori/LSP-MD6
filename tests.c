#define _GNU_SOURCE 1

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ftw.h>

#include "tests.h"

#define EXT_UNKNOWN -1
#define EXT_CHUNK 0
#define EXT_REQUEST 1

TestNode * test_head = NULL;
TestNode * test_tail = NULL;

static TestNode * get_by_name(char * name)
{
    TestNode * result;

    result = test_head;
    while(result) {
        if(!strcmp(result->name, name))
            break;
        result = result->next;
    }

    return result;
}

static int scan_callback(const char *fpath, const struct stat *sb, int tflag,
                        struct FTW *ftwbuf) {

    TestNode * test;
    char * last_dot, * name;
    unsigned int path_size, name_size;
    int ext = EXT_UNKNOWN;

    //Skip every thing that is not a file.
    if(tflag != FTW_F)
        return 0;

    //Measure path size.
    path_size = strlen(fpath) + 1;

    //Get last dot, (start of extension).
    last_dot = strrchr(fpath + ftwbuf->base, '.');
    name_size = last_dot - (fpath + ftwbuf->base) + 1;

    //Figure out the extension.
    if(!strcmp(last_dot + 1, "chunk")) {
        ext = EXT_CHUNK;
    } else if(!strcmp(last_dot + 1, "request")) {
        ext = EXT_REQUEST;
    } else {
        //Skip, unknown extension.
        return 0;
    }

    //Check if we have this test already.
    name = (char *) calloc(name_size, sizeof(char));
    memcpy(name, fpath + ftwbuf->base, name_size - 1);
    name[name_size - 1] = '\0';

    test = get_by_name(name);

    if(!test) {
        //Create a new test node.
        test = (TestNode *) malloc(sizeof(TestNode));
        test->name = name;
        test->next = NULL;

        if(test_tail) {
            test_tail->next = test;
        } else {
            test_head = test;
        }

        test_tail = test;
    } else {
        //We found a test by that name, so we can free the name.
        free(name);
    }

    switch (ext) {
        case EXT_CHUNK:
            //Don't overwrite it.
            if (test->chunk_path == NULL) {
                test->chunk_path = (char *) calloc(path_size, sizeof(char));
                memcpy(test->chunk_path, fpath, path_size);
            }
            break;
        case EXT_REQUEST:
            //Don't overwrite it.
            if (test->request_path == NULL) {
                test->request_path = (char *) calloc(path_size, sizeof(char));
                memcpy(test->request_path, fpath, path_size);
            }
            break;
    }

    return 0;
}

TestNode * tests_get(char * test_dir)
{
    test_head = NULL;
    test_tail = NULL;

    nftw(test_dir, scan_callback, 20, FTW_PHYS);

    return test_head;
}

void tests_free(TestNode * test_head)
{
    TestNode * test_temp;
    //Free tests.
    while(test_head) {
        test_temp = test_head->next;
        free(test_head);
        test_head = test_temp;
    }
}
