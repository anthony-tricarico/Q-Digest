#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/tree_reduce.h"
#include "../../include/memory_utils.h"
#include "../../include/qcore.h"

/* NOTE: These are test parameters and should be removed in 
 * favor of proper user-based I/O */
// how many numbers to generate
// also the size of the array (vector) that stores them in process 0
#define DATA_SIZE 10
#define K 5
#define LOWER_BOUND 0
#define UPPER_BOUND 20

/* ============== MAIN FUNCTION ======================== */
int main(void) 
{
    int rank, comm_sz;

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
        initialize_data_array(rank, buf, DATA_SIZE, LOWER_BOUND, UPPER_BOUND);
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

    printf("DEBUG: rank %d Distribution is ok\n", rank);

    // From the data buffer create the q-digest
    size_t local_upper_bound = _get_curr_upper_bound(local_buf, local_n);
    size_t global_upper_bound;
    MPI_Allreduce(
        &local_upper_bound,
        &global_upper_bound,
        1,
        MPI_UNSIGNED_LONG,
        MPI_MAX,
        MPI_COMM_WORLD
    );
    printf("CRITICAL: rank %d and upper bound is %zu\n", rank, global_upper_bound);
    // This function is here to simulate the computational cost required to find the current upper bound in the data, since in 
    // an actual implementation and execution we don't know a priori the real upper bound in the data. 
    struct QDigest *q = _build_q_from_vector(local_buf, local_n, global_upper_bound, K);

    printf("DEBUG: q-digest build in rank %d succeded\n", rank);

    // data get inserted into qdigest and then compressed, ecc...
    tree_reduce(q, comm_sz, rank, MPI_COMM_WORLD);
    printf("DEBUG: tree reduce in rank %d completed\n", rank);

    if (rank == 0) {
        size_t res = percentile(q, 0.5);
        printf("result is %lu\n", res);
    }

    MPI_Finalize();
    return 0;
}
