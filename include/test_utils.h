#ifndef TEST_UTILS
#define TEST_UTILS

#include "../include/dynamic_array.h"

/* This is a helper function that is used by the qsort function */
int comp(const void *a, const void *b);


/* This is a function that shuffles a given array */
void shuffle(DAItem *array, size_t n);

/* This function prints the real vs computed percentiles according to QDigest
 * Args:
 *  double p: the percentile to compute (a value between 0 and 1, inclusive)
 *  const Array *a: a pointer to a variable of type array containing the data
 *  struct QDigest *q: a pointer to the QDigest structure
 * Returns:
 *  Nothing */
void compare_percentiles(double p, const Array *a, struct QDigest *q);

/* This function tests a QDigest against a Poisson distribution */
void test_poisson_distribution(int n, int k, int seed);

void test_geometric_distribution(int n, int k, int seed);

double compute_compression_ratio(struct QDigest *q);

void test_random_distribution(int n, int k, int seed);

void test_merge(int n, int k, int seed);
 
#endif
