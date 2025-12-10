#ifndef OMPCORE_H
#define OMPCORE_H

#include <stdlib.h>
#include <omp.h>
#include "../../include/qcore.h"

/**
 *  @brief Fills a buffer with pseudo-random integers for a given thread.
 *
 *  Seeds the C RNG with @p rank so every process chosen for the generation
 *  (default case is the master thread) creates a reproducible sequence, then 
 *  writes @p n values between the given @lb and @ub (lower bound and upper bound)
 *  (inclusive) into @p data.
 *
 *  @param thread_id Calling thread ID; used both for RNG seeding and to decorrelate buffers.
 *  @param data Pointer to the buffer that will receive @p n integers.
 *  @param n Number of elements to generate.
 *  @param lb lower bound of data.
 *  @param up upper bound of data.
 *
 */
void initialize_data_array_for_omp(
    int thread_id,
    int *data,
    size_t n,
    size_t lb,
    size_t ub);

#endif /* OMPCORE_H */
