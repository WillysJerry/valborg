#ifndef THREADING_H
#define THREADING_H

#include "parallel.h"
#include <runtime.h>

//#define NUM_THREADS 4

// As a starting point, data-parallel operations will only operate on the set of indices that intersect the input arrays. Perhaps we'd want to change this later, but how would we handle indices outside the range of one of the arrays?
typedef struct _distribution {
	int m, n;
	int n_arrs;			// Number of arrays
	int b_size[NUM_THREADS];	// Size of each block	
	const par_array* arrs;		// As many arrays as n_arrs
	int blocks[NUM_THREADS];	// Starting indices of the blocks
	int size;			// Amount of elements in the individual arrays
} distribution;

//distribution distribute(const par_array* arr, int n, unsigned char mode);
distribution distribute(const par_array* arr, int n_arrays, int m, int n);
void set_dist_size(distribution* dist, int m, int n);
void free_distribution(distribution dist);
void print_distribution(distribution dist);

int global_to_local_block(const distribution a, int block_id, int i);
int local_to_global_block(const distribution a, int block_id, int i);

#include "threadpool/threadpool.h"

#endif // THREADING_H
