#ifndef THREADING_H
#define THREADING_H

#include "../parallel.h"
#include "../runtime.h"

#define NUM_THREADS 4

#define DISTRIBUTION_STRICT 		0	// All input arrays must have the same m..n
#define DISTRIBUTION_INTERSECTION	1	// The distribution will cover the intersecting indices of the arrays
#define DISTRIBUTION_UNION		2
#define DISTRIBUTION_SUM		3	// The distribution will cover the sum of the arrays index ranges 

// As a starting point, data-parallel operations will only operate on the set of indices that intersect the input arrays. Perhaps we'd want to change this later, but how would we handle indices outside the range of one of the arrays?
typedef struct _distribution {
	int m, n;
	int n_arrs;			// Number of arrays
	int b_size[NUM_THREADS];	// Size of each block	
	const par_array* arrs;		// As many arrays as n_arrs
	int blocks[NUM_THREADS];	// Starting indices of the blocks
	int size;			// Amount of elements in the individual arrays
} distribution;

distribution distribute(const par_array* arr, int n, unsigned char mode);
void free_distribution(distribution dist);
void print_distribution(distribution dist);

int global_to_local_block(const distribution a, int block_id, int i);
int local_to_global_block(const distribution a, int block_id, int i);

void init_threadpool();
void kill_threadpool();

par_array execute_in_parallel(void (*work)(distribution dist, int id, par_array* out, void* f, void* p, void* args), distribution dist, int out_m, int out_n, void* f, void* p, void* args);

void barrier();

#endif // THREADING_H
