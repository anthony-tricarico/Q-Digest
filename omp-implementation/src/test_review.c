#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../include/ompcore.h"
#include "../../include/memory_utils.h"
#ifdef _OPENMP
    #include <omp.h>
#endif

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
int main(int argc, char* argv[]) {

    if (argv[1] == NULL) {
        fprintf(stderr, "Please provide thread_count in input\n");
        return 1;
    } 
    
    /* Get number of threads from command line */
    int thread_count  = strtol(argv[1], NULL, 10);
    printf("Thread count is %d\n", thread_count);

    /* Compiler CLI example  */
    /* $ gcc-15 -g -Wall -fopenmp 
     *      ./src/test_review.c ./src/ompcore.c 
     *      ../src/memory_utils.c -o test_try
     *  $ ./omp_hello 4
     */ 

    /* Handling possible rest division in n/comm_sz */
    int base = DATA_SIZE / thread_count; 
    int rest = DATA_SIZE % thread_count;

    /* Creating shared-memory indexes to divide
     * data among threads
     */
    int *counts = xmalloc(thread_count*sizeof(int));
    int *displs = xmalloc(thread_count*sizeof(int));
    int offset = 0;
    // printf("Data size: %d\n", DATA_SIZE);
    for (int i = 0; i < thread_count; i++) {
        counts[i] = base + (i < rest ? 1 : 0);
        displs[i] = offset;
        printf("Counts[%d]: %d\n", i, counts[i]);
        printf("Displs[%d]: %d\n", i, displs[i]);
        offset += counts[i];
    }

    int *buf = xmalloc(DATA_SIZE*sizeof(int));
    #pragma omp master  
    {
        int master_id = omp_get_thread_num();
        initialize_data_array_for_omp(master_id, buf, DATA_SIZE, LOWER_BOUND, UPPER_BOUND);
    }   

    // Variables and containers for parallel directive
    size_t n = DATA_SIZE-1;
    size_t *upper_bounds = xmalloc(thread_count*sizeof(size_t));
    size_t glob_mx = 0;

    #pragma omp parallel num_threads(thread_count) \
        default(none) shared(buf, counts, displs, \
                upper_bounds, n, thread_count, glob_mx)
    {
        int thread_id = omp_get_thread_num();
        size_t thread_ix_start = displs[thread_id];
        size_t thread_ix_end;
        if (thread_id+1 == thread_count) {
            thread_ix_end = n;
        } else {
            thread_ix_end = displs[thread_id+1];
        }
        printf("For thread %d data start at %zu and end at %zu\n", 
                thread_id, thread_ix_start, thread_ix_end);
        
        // Private local upper bound
        size_t local_mx = 0;
        for (size_t i = thread_ix_start; 
                i < thread_ix_end; i++) {
            if (buf[i] > local_mx) local_mx = buf[i]; 
        }
        // False sharing overhead accepted,
        // collecting local upper boundaries
        upper_bounds[thread_id] = local_mx;
        #pragma omp barrier

        // Reduce local upper boundaries array to global upper boundary
        #pragma omp master 
        {
            for (int thr_i = 0; thr_i < thread_count; thr_i++) {
                if (glob_mx < upper_bounds[thr_i]) glob_mx = upper_bounds[thr_i];
                printf("Thread %d 's local UB is %zu\n", thr_i, upper_bounds[thr_i]);
            }
            printf("Global upper boundary is: %zu\n", glob_mx);
        }
    }
    return 0;
}
