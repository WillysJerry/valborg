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

distribution distribute(const par_array* arr, int n, unsigned char mode) {
	int 	i,
		a_m, 		// The "m" of the distribution, the lowest intersecting index of all input arrays
		a_n, 		// The "n" of the distribution, the largest intersecting index of all input arrays
		len, 		// The total length of the segment (intersecting indices)
		block_size;	// Size of the individual blocks
	distribution dist;

	a_m = 0; a_n = 0;
	if(mode == DISTRIBUTION_STRICT) {
		a_m = arr[0].m;
		a_n = arr[0].n;
		for(i = 1; i < n; i++) {
			if(arr[i].m != a_m || arr[i].n != a_n) {
				dist.n_arrs = 0;
				return dist;
			}
		}
	}
	else if(mode == DISTRIBUTION_INTERSECTION) {
		// Figure out where the arrays intersect
		a_m = arr[0].m;
		a_n = arr[0].n;
		for(i = 1; i < n; i++) {
			if(arr[i].m > a_m)
				a_m = arr[i].m;	
			if(arr[i].n < a_n)
				a_n = arr[i].n;
		}
		dist.m = a_m;
		dist.n = a_n;

		// m should always be less then n if any of the indices were intersecting.
		if(a_m > a_n) {
			// Return an empty distribution
			dist.n_arrs = 0;
			return dist;
		}
	}
	else if(mode == DISTRIBUTION_UNION) {
		// Get lowest m and highest n 
		a_m = arr[0].m;
		a_n = arr[0].n;
		for(i = 1; i < n; i++) {
			if(arr[i].m < a_m)
				a_m = arr[i].m;	
			if(arr[i].n > a_n)
				a_n = arr[i].n;
		}
		dist.m = a_m;
		dist.n = a_n;
	}
	else if(mode == DISTRIBUTION_SUM) {
		a_m = arr[0].m;
		int size = arr[0].n - arr[0].m + 1;
		for(i = 1; i < n; i++) {
			if(arr[i].m < a_m)
				a_m = arr[i].m;	
			size += arr[i].n - arr[i].m + 1;
		}

		a_n = a_m + size - 1;

	}

	dist.n_arrs = n;
	
	len = a_n - a_m + 1;			// Total length of the array segment
	dist.size = len;

	block_size 	= len / NUM_THREADS;	// The minimum block size 
	int remainder 	= len % NUM_THREADS;

	// Set block sizes for the remaining threads
	int cur_size;		// Size of the current block when iterating
	int acc = 0;		// Accumulator to figure out the starting indices of the blocks

	for(i = 0; i < NUM_THREADS; i++) {
		cur_size = block_size;
		if(i < remainder)
			cur_size += 1;

		dist.blocks[i] = acc;
		dist.b_size[i] = cur_size;
		acc += cur_size;
	}

	// Store references to the arrays
	/*dist.arrs = (double**)calloc(n, sizeof(double*));
	for(i = 0; i < n; i++) {
		dist.arrs[i] = arr[i].a 
			+ (a_m - arr[i].m);	// Need to offset the array by the lowest intersecting m (a_m) relative to its starting point (arr[i].m).
	}*/

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
