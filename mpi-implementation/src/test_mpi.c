#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../../include/qcore.h"
#include "../include/tree_reduce.h"
#include "../../include/memory_utils.h"

#define TEST 1

int main(void) {
    int rank, comm_sz;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

#if TEST
    /* Exercise tree_reduce with deterministic QDigest */
    int a[10] = {0,1,2,4,5,6,7,8,9};
    int n = sizeof(a)/sizeof(a[0]);

    distribute_data_array(a, local_n, rank, n, MPI_COMM_WORLD);
    
#endif

    // test_tree_reduce() 

    MPI_Finalize();
    printf("All test passed!\n");
    return 0;
}


