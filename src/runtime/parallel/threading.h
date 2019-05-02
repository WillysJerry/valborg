#ifndef THREADING_H
#define THREADING_H

#include "../parallel.h"
#include "../runtime.h"

#define NUM_THREADS 4

// As a starting point, data-parallel operations will only operate on the set of indices that intersect the input arrays. Perhaps we'd want to change this later, but how would we handle indices outside the range of one of the arrays?
typedef struct _distribution {
	int m;
	int n_arrs;			// Number of arrays
	int b_size[NUM_THREADS];	// Size of each block	
	double** arrs;			// As many arrays as n_arrs
	int blocks[NUM_THREADS];	// Starting indices of the blocks
} distribution;

typedef struct _dist_ret {
	double* v;
	int n;
} dist_ret;

distribution distribute(const par_array* arr, int n);
void free_distribution(distribution dist);
void print_distribution(distribution dist);

dist_ret** execute_in_parallel(void (*work)(distribution dist, int id, dist_ret* retval, void* f, void* p), distribution dist, void* f, void* p);

void merge_result(dist_ret** ret, par_array* result);

#endif // THREADING_H
