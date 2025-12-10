#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "../include/ompcore.h"
#include "../../include/memory_utils.h"
#include "../../include/qcore.h"

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

struct QDigest *_build_q_from_vector_for_omp(
        int *a,
        size_t start,
        size_t end,
        int size, 
        size_t upper_bound, 
        int k) 
{
    /* FIXED: This portion of the code was causing a segfault
     * due to the fact that when using an upper bound that is much
     * smaller than the actual received number the q-digest might
     * overflow internal nodes, causing a strange ranges in serialization. */
    struct QDigest *q = create_tmp_q(k, upper_bound);
    for (size_t i = start; i < end; i++) {
        insert(q, a[i], 1, true);
    }
    return q;
}


