#include "../parallel.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <math.h>

// Evaluates one level of the reduction tree
void reduce_thrd(distribution dist, int id, dist_ret* retval, void* f, void* p) {
	int size = dist.b_size[id];
	double* res = (double*)calloc(1, sizeof(double));

	double* A = dist.arrs[0] + dist.blocks[id];
	*res = A[0]; 

	// Handle base cases
	if(size == 1) {
		retval->v = res;
		retval->n = 1;
		return;
	} else if(size == 0) {
		return;
	}

	// Cast void pointers to functions, this is not very readable
	double (*func)(double x, double y) =
		(double (*)(double x, double y)) f;
	int (*pred)(int i) =
		(int (*)(int)) p;

	for(int i = 1; i < size; i++) {
		*res = func(*res, A[i]);	
	}
	retval->v = res;
	retval->n = 1;
	/*
	int i, j;
	for(i = 1, j = 0; i < size; i += 2, j++) {
		arr[j] = func(A[i-1], A[i]);
	}

	if(size % 2 != 0)
		arr[j] = A[i-1];

	retval->v = arr;
	retval->n = res_size;
	*/
}

double par_reduce(double (*f)(double x, double y), const par_array a) {
	distribution dist;
	double result = 0.0;

	dist_ret** ret = NULL;
	par_array res_array;

	dist = distribute(&a, 1);

	// TODO: Add predicate p to the expression below
	ret = execute_in_parallel(reduce_thrd, dist, (void*)f, NULL);
	merge_result(ret, &res_array);
	free(ret);
	free_distribution(dist);

	result = res_array.a[0];
	for(int i = 1; i < length(res_array); i++) {
		result = f(result, res_array.a[i]);	
	}

	return result;
}
