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

int global_to_local_block(const distribution a, int block_id, int i);
int local_to_global_block(const distribution a, int block_id, int i);

void init_threadpool();
void kill_threadpool();

void execute_in_parallel(void (*work)(int i, par_array* input, void* output, void* f, void* p, void* args), const par_array* input, void* output, int m, int n_elements, void* f, void* p, void* args);

void merge_result(dist_ret** ret, par_array* result);

void barrier();

#endif // THREADING_H
