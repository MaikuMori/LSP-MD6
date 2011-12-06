#pragma once

#include <stdio.h>
#include "main.h"

int alg_first_fit(MemoryBlock * memorylist_head, int * requests, int total);
int alg_next_fit(MemoryBlock * memorylist_head, int * requests, int total);
int alg_best_fit(MemoryBlock * memorylist_head, int * requests, int total);
int alg_worst_fit(MemoryBlock * memorylist_head, int * requests, int total);
