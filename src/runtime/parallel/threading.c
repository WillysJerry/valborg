#include "threading.h"

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>

#include "../runtime.h"
#include "../parallel.h"

pthread_t thrds[NUM_THREADS];

// Args to be passed to the worker functions assigned to each thread.
typedef struct _worker_args {
	int id;
	void (*work)(distribution dist, int id, dist_ret* retval, void* f, void* p);	// Work to be executed on each thread
	distribution dist;
	void* f;				// Optional function
	void* p;				// Optional predicate
} worker_args;

distribution distribute(const par_array* arr, int n) {
	int 	i,
		a_m, 		// The "m" of the distribution, the lowest intersecting index of all input arrays
		a_n, 		// The "n" of the distribution, the largest intersecting index of all input arrays
		len, 		// The total length of the segment (intersecting indices)
		block_size;	// Size of the individual blocks
	distribution dist;

	// Figure out where the arrays intersect
	a_m = 0;
	a_n = length(arr[0]) - 1;
	for(i = 0; i < n; i++) {
		if(arr[i].m > a_m)
			a_m = arr[i].m;	
		if(arr[i].n < a_n)
			a_n = arr[i].n;
	}
	dist.m = a_m;

	// m should always be less then n if any of the indices were intersecting.
	if(a_m > a_n) {
		// Return an empty distribution
		dist.n_arrs = 0;
		return dist;
	}

	dist.n_arrs = n;
	
	len = a_n - a_m + 1;			// Total length of the array segment
	block_size = (len) / NUM_THREADS;	// Size of all blocks except the first one

	// TODO: Perhaps optimize this somewhat. First processor might be assigned (marginally) more load then the others depending on how many workers are available.
	dist.b_size[0] = block_size + (len % NUM_THREADS);	// First block also needs to account for uneven length
	dist.blocks[0] = 0;					// First block begins at 0

	// Set block sizes for the remaining threads
	int acc = dist.b_size[0];	// Accumulator to figure out the starting indices of the rest of the blocks
	for(i = 1; i < NUM_THREADS; i++) {
		dist.blocks[i] = acc;
		dist.b_size[i] = block_size;
		acc += block_size;
	}

	// Store references to the arrays
	dist.arrs = (double**)calloc(n, sizeof(double*));
	for(i = 0; i < n; i++) {
		dist.arrs[i] = arr[i].a 
			+ (a_m - arr[i].m);	// Need to offset the array by the lowest intersecting m (a_m) relative to its starting point (arr[i].m).
	}

	return dist;
}

void free_distribution(distribution dist) {
	free(dist.arrs);
}

void print_distribution(distribution dist) {
	int indx;
	for(int i = 0; i < dist.n_arrs; i++) {
		indx = 0;
		printf("[%d] ", i);
		for(int j = 0; j < NUM_THREADS; j++) {
			printf("| ");
			for(int k = 0; k < dist.b_size[j]; k++) {
				printf("%.2f ", dist.arrs[i][indx]);
				indx++;
			}
		}
		printf("|\n");
	}
}

//void* worker(int id, void(*f)(void* dist)) {
void* worker(void* args) {
	worker_args* a = (worker_args*)args;
	dist_ret* retval = (dist_ret*)calloc(1, sizeof(dist_ret));
	retval->n = 0;
	a->work(a->dist, a->id, retval, a->f, a->p);

	if(retval->n <= 0) {
		free(retval);
		retval = NULL;
	}
	pthread_exit((void*)retval);
}

dist_ret** execute_in_parallel(void (*work)(distribution dist, int id, dist_ret* retval, void* f, void* p), distribution dist, void* f, void* p) {
	int s = 0;
	printf("#### CREATING THREADS ####\n");
	worker_args* args = (worker_args*)calloc(NUM_THREADS, sizeof(worker_args));
	for(int i = 0; i < NUM_THREADS; i++) {
		args[i].id = i;
		args[i].work = work;
		args[i].dist = dist;
		args[i].f = f;
		args[i].p = p;

		s = pthread_create(&thrds[i], NULL, worker, (void*)(args + i));
		if(s == 0) {
			printf("Thread %d started successfully\n", i);
		}
	}

	printf("#### BARRIER ####\n");
	dist_ret** ret = (dist_ret**)calloc(NUM_THREADS, sizeof(dist_ret*));
	
	for(int i = 0; i < NUM_THREADS; i++) {
		ret[i] = NULL;
		pthread_join(thrds[i], (void**)(ret + i));

		// Something was returned?
		if(ret[i] != NULL) {
			printf("Thread %d returned something %f\n", i, ret[i]->v[0]);
		}
	}
	printf("#### PARALLEL EXECUTION DONE ####\n");

	free(args);

	return ret;
}
