#include "threading.h"

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>

#include "../runtime.h"
#include "../parallel.h"

void set_dist_size(distribution* dist, int m, int n) {
	int len = n - m + 1;			// Total length of the array segment
	dist->size = len;
	dist->m = m;
	dist->n = n;

	int block_size 	= len / NUM_THREADS;	// The minimum block size 
	int remainder 	= len % NUM_THREADS;

	// Set block sizes for the remaining threads
	int cur_size;		// Size of the current block when iterating
	int acc = 0;		// Accumulator to figure out the starting indices of the blocks

	for(int i = 0; i < NUM_THREADS; i++) {
		cur_size = block_size;
		if(i < remainder)
			cur_size += 1;

		dist->blocks[i] = acc;
		dist->b_size[i] = cur_size;
		acc += cur_size;
	}

}

distribution distribute(const par_array* arr, int num_arrays, int m, int n) {
	distribution dist;
	dist.m = m;
	dist.n = n;
	dist.n_arrs = num_arrays;
	
	int len = n - m + 1;			// Total length of the array segment
	dist.size = len;

	int block_size 	= len / NUM_THREADS;	// The minimum block size 
	int remainder 	= len % NUM_THREADS;

	// Set block sizes for the remaining threads
	int cur_size;		// Size of the current block when iterating
	int acc = 0;		// Accumulator to figure out the starting indices of the blocks

	for(int i = 0; i < NUM_THREADS; i++) {
		cur_size = block_size;
		if(i < remainder)
			cur_size += 1;

		dist.blocks[i] = acc;
		dist.b_size[i] = cur_size;
		acc += cur_size;
	}

	dist.arrs = arr;
	return dist;
}

void free_distribution(distribution dist) {
	//free(dist.arrs);
}

int global_to_local_block(const distribution a, int block_id, int i) {
	return i - a.m - a.blocks[block_id];

}
int local_to_global_block(const distribution a, int block_id, int i) {
	return a.m + a.blocks[block_id] + i;

}

void print_distribution(distribution dist) {
	int indx;
	for(int i = 0; i < dist.n_arrs; i++) {
		indx = 0;
		printf("[%d] ", i);
		for(int j = 0; j < NUM_THREADS; j++) {
			printf("| ");
			for(int k = 0; k < dist.b_size[j]; k++) {
				printf("%.2f ", VAL(dist.arrs[i].a[indx]));
				indx++;
			}
		}
		printf("|\n");
	}
}
