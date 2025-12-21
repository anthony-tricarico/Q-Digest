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

/**
 *  @brief Build a QDigest from an integer vector.
 *
 *  Allocates and initializes a QDigest sized for @p upper_bound, then inserts
 *  the @p size values from @p a into it, returning the populated digest.
 *  While in the message-passing set-up the buffer provided correspond with
 *  the local buffer of the node, in the shared-memory set up, a pointer to
 *  the local buffer is passed along with @p start and @p end, which indicate 
 *  the portion of the buffer being processed by the thread.
 *
 *  @param a           Pointer to the input array of integers.
 *  @param start       Zero-based index identifying the first element of the segment to process.
 *  @param end         Zero-based index identifying the boundary one past the last element of the segment (exclusive).
 *  @param size        Number of elements in @p a.
 *  @param upper_bound Maximum value expected in @p a (used to size the digest).
 *
 *  @return Newly allocated QDigest containing all values from @p a.
 */
struct QDigest *_build_q_from_vector_for_omp(
        int *a,
        size_t start,
        size_t end,
        int size, 
        size_t upper_bound, 
        int k);


#endif /* OMPCORE_H */
