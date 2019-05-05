#include "../parallel.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <math.h>
// Masks out elements based on input predicate from the array before performing the scan
void scan_msk_thrd(distribution dist, int id, dist_ret* retval, void* f, void* p, void* args) { 
	int size = dist.b_size[id];
	double* arr = (double*)calloc(dist.b_size[id], sizeof(double));

	// Just assume that these arrays were passed...
	double* A = dist.arrs[0] + dist.blocks[id];

	// Cast void pointers to functions, this is not very readable
	int (*pred)(int i) =
		(int (*)(int)) p;

	for(int i = 0; i < size; i++) {
		if(pred == NULL || pred(local_to_global_block(dist, id, i))) {
			arr[i] = A[i];
		} else {
			arr[i] = 0.0;	// Zero masked elements, maybe something else should happen? 
		}
	}

	retval->v = arr;
	retval->n = size;
}

void scan_thrd(distribution dist, int id, dist_ret* retval, void* f, void* p, void* args) {
	int size = dist.b_size[id];
	double* arr = (double*)calloc(dist.b_size[id], sizeof(double));

	// Just assume that these arrays were passed...
	double* A = dist.arrs[0] + dist.blocks[id];

	// Cast void pointers to functions, this is not very readable
	double (*func)(double x, double y) =
		(double (*)(double x, double y)) f;

	int* lvl = (int*)args;
	int k = 0x1 << *lvl;		// 2^j 

	int glob = local_to_global_block(dist, id, 0);
	for(int i = 0; i < size; i++) {
		if(glob + i >= k)
			arr[i] = func(A[i - k], A[i]);
		else
			arr[i] = A[i];
	}

	retval->v = arr;
	retval->n = size;
}

par_array par_scan(double (*f)(double x, double y), const par_array a, int (*p)(int i)) {
	distribution dist;

	dist_ret** ret = NULL;
	par_array res_array;


	if(p != NULL) {
		dist = distribute(&a, 1);
		ret = execute_in_parallel(scan_msk_thrd, dist, NULL, (void*)p, NULL);
		merge_result(ret, &res_array);
		free(ret);
		free_distribution(dist);
	} else {
		res_array = a;
	}

	for(int i = 0; i < ceil(log2(length(a))); i++) {
		dist = distribute(&res_array, 1);

		// TODO: Need to shrink the area that gets operated on in parallel at every
		// iteration so that we don't need to go through the entire array every time.
		ret = execute_in_parallel(scan_thrd, dist, (void*)f, NULL, (void*)&i);
		merge_result(ret, &res_array);
		free(ret);
		free_distribution(dist);
	}

	return res_array;
}
