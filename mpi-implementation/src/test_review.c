/** 
 *  @file test_review.c
 *  @brief This file contains a small test carried out to check
 *  that the end-to-end workflow would work as expected. The
 *  operations performed in this file replicate what would happen
 *  normally when a quantile query is issued and data from different
 *  nodes should be retrieved and aggregated according to the rules
 *  set by the Q-Digest data structure.
 *  On a more technical note, the algorithm works as follows:
 *  1. Data is simulated and sent from process 0 to each node
 *  making sure that each process receives equally-sized
 *  integer-valued vectors. This is achieved by using
 *  `MPI_Scatterv`.
 *  2. The maximum `upper_bound` of the data distribution (i.e., the
 *  largest integer contained in the vector) is extracted and
 *  it is compared to all the other maximums extracted from the other
 *  processes. The largest integer among those is saved in the variable
 *  `global_upper_bound` with the use of `MPI_Allreduce`.
 *  3. Each process builds its own Q-Digest from the vector of data
 *  received, using the `global_upper_bound` as its `upper_bound` parameter.
 *  4. A binary tree reduce is performed to merge all Q-Digests at each
 *  step with other Q-Digest in their relative partner processes until
 *  a single Q-Digest is created and can be queried to answer the quantile
 *  query that was originally requested by the user.
 *
 */
#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../include/tree_reduce.h"
#include "../../include/memory_utils.h"
#include "../../include/qcore.h"

/* NOTE: These are test parameters and should be removed in 
 * favor of proper user-based I/O */
// how many numbers to generate
// also the size of the array (vector) that stores them in process 0
#define DATA_SIZE 300
#define K 200   // higher values of K provide lesser compression but more accuracy
#define LOWER_BOUND 0
#define UPPER_BOUND 20


int compute_median(int *buf, size_t size);
/* ============== MAIN FUNCTION ======================== */
int main(void) 
{
    int rank, comm_sz;
    size_t global_upper_bound;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    /* Handling possible rest division in n/comm_sz */
    int base = DATA_SIZE/comm_sz;
    int rest = DATA_SIZE % comm_sz;

    /* Filling counts and displacements arrys with counts of 
    how many integers each nodes will receive and with offesets
    describing how to scatter the original array into the 
    different processes. */
    int *counts = xmalloc(comm_sz*sizeof(int));
    int *displs = xmalloc(comm_sz*sizeof(int));
    int offset = 0;
    for (int i = 0; i < comm_sz; i++) {
        counts[i] = base + (i < rest ? 1 : 0);
        displs[i] = offset;
        offset += counts[i];
    }

    /* Computing local dimension of the array for the given rank
    and allocating proper memory space to held the scattered data. */
    int local_n = counts[rank];
    int *local_buf = xmalloc(local_n*sizeof(int));

    /* This is initializing the array, technically only rank 0
     * should initialize the array and scatter it around */
    
    int *buf = xmalloc(DATA_SIZE*sizeof(int));
    if (rank == 0) {
        #ifdef TEST
        for (int i = 0; i < DATA_SIZE; i++) {
            buf[i] = i;
        }
        #else
        initialize_data_array(rank, buf, DATA_SIZE, LOWER_BOUND, UPPER_BOUND);
        #endif
    }

    // printf("DEBUG: Before barrier\n");
    /* Synchronizes all processes to wait for the buf to
     * be scattered to be populated by process 0 */
    // MPI_Barrier(MPI_COMM_WORLD);

    /* Scatter the array around the nodes */ 
    distribute_data_array(
        buf, 
        local_buf,
        counts,
        displs,
        local_n,
        rank,
        DATA_SIZE,
        MPI_COMM_WORLD
    );

    // printf("DEBUG: rank %d Distribution is ok\n", rank);

    // From the data buffer create the q-digest
    size_t local_upper_bound = _get_curr_upper_bound(local_buf, local_n);
    MPI_Allreduce(
        &local_upper_bound,
        &global_upper_bound,
        1,
        MPI_UNSIGNED_LONG,
        MPI_MAX,
        MPI_COMM_WORLD
    );
    // printf("CRITICAL: rank %d and upper bound is %zu\n", rank, global_upper_bound);
    // This function is here to simulate the computational cost required to find the current upper bound in the data, since in 
    // an actual implementation and execution we don't know a priori the real upper bound in the data. 
    struct QDigest *q = _build_q_from_vector(local_buf, local_n, global_upper_bound, K);

    // printf("DEBUG: q-digest build in rank %d succeded\n", rank);

    // data get inserted into qdigest and then compressed, ecc...
    tree_reduce(q, comm_sz, rank, MPI_COMM_WORLD);
    // printf("DEBUG: tree reduce in rank %d completed\n", rank);

# ifdef TEST
   /* ============== TESTS ============= */
    // check if size of the communicator is as expected
    if (rank == 0) {
        size_t min = percentile(q, 0);
        size_t max = percentile(q, 1);
        size_t median = percentile(q, 0.5);
        printf("max is %zu\n", max);
        printf("min is %zu\n", min);

        int actual_median = compute_median(buf, DATA_SIZE);
        printf("median is %zu\n", median);
        printf("actual median is %d\n", actual_median);
        printf("q-digest is off by %d\n", abs(median-actual_median));
        assert(min == 0);
        assert(max == DATA_SIZE-1);
        // assert(median == actual_median);
        printf("SUCCESS: Assertion tests passed on deterministic array\n");
    }
# endif

    MPI_Finalize();
    return 0;
}

/* compute median for deterministic array which 
 * is already sorted! If the array is not sorted
 * this function will not work. */
int compute_median(int *buf, size_t size) {
    int idx;
    if (size % 2 == 0) { // even branch
        idx = ceil((double)(size / 2));
        return ceil((buf[idx]+buf[idx+1])/2);
    } else {
        idx = ceil((double)(size/2));
        return buf[idx];
    }
}
