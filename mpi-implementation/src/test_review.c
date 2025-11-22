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
#define DATA_SIZE 1024
#define K 5

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

    int arr[DATA_SIZE];
    if (rank == 0) {
        for (int i = 0; i < DATA_SIZE; i++) {
            arr[i] = i;
        }
    }
    /* Scatter the array around the nodes */ 
    MPI_Barrier(MPI_COMM_WORLD);
    distribute_data_array(
        arr, 
        local_buf,
        counts,
        displs,
        local_n,
        rank,
        DATA_SIZE,
        true,
        MPI_COMM_WORLD
    );

    // From the data buffer create the q-digest
    size_t upper_bound = _get_curr_upper_bound(local_buf, local_n);
    struct QDigest *q = _build_q_from_vector(local_buf, local_n, upper_bound, K);

    // data get inserted into qdigest and then compressed, ecc...
    tree_reduce(q, comm_sz, rank, MPI_COMM_WORLD);

    if (rank == 0) {
        size_t res = percentile(q, 0.5);
        printf("result is %lu\n", res);
    }

    MPI_Finalize();
    return 0;
}
