#pragma once

#define TEST_PATH "./tests"

typedef struct _TestNode {
    char * name;
    char * chunk_path;
    char * request_path;

    struct _TestNode * next;
} TestNode;

TestNode * tests_get(char * test_dir);
void tests_free(TestNode * test_head);
