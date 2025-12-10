#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "../include/ompcore.h"
#include "../../include/memory_utils.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b));

void initialize_data_array_for_omp(
    int rank,
    int *data,
    size_t n,
    size_t lb,
    size_t ub)
{
    int i;
    srand(rank);
    for (i = 0; i < n; i++)
        data[i] = lb + rand() % (ub-lb + 1);
    return;
}


