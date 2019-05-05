#include "../parallel.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <math.h>

// Evaluates one level of the reduction tree
void reduce_thrd(distribution dist, int id, dist_ret* retval, void* f, void* p, void* args) {
	int i;
	double* res = NULL;
	int size = dist.b_size[id];

	double* A = dist.arrs[0] + dist.blocks[id];
	int arrbase = dist.m + dist.blocks[id];
	//*res = A[0]; 
	
	// Cast p pointer to predicate function
	int (*pred)(int i) =
		(int (*)(int)) p;

	for(i = 0; i < size; i++) {
		if(pred == NULL || pred(i + arrbase)) {
			res = (double*)calloc(1, sizeof(double));
			*res = A[i]; 
			break;
		}
	}

	// Handle base cases
	if(size == 0 || res == NULL) {
		return;
	}
	else if(size == 1) {
		retval->v = res;
		retval->n = 1;
		return;
	}

	// Cast void pointers to functions, this is not very readable
	double (*func)(double x, double y) =
		(double (*)(double x, double y)) f;

	for(i += 1; i < size; i++) {
		if(pred == NULL || pred(i + arrbase)) {
			*res = func(*res, A[i]);	
		}
	}

	retval->v = res;
	retval->n = 1;
}

double par_reduce(double (*f)(double x, double y), const par_array a, int (*p)(int i)) {
	distribution dist;
	double result = 0.0;

	dist_ret** ret = NULL;
	par_array res_array;

	dist = distribute(&a, 1);

	ret = execute_in_parallel(reduce_thrd, dist, (void*)f, (void*)p, NULL);
	merge_result(ret, &res_array);
	free(ret);
	free_distribution(dist);

	result = res_array.a[0];
	for(int i = 1; i < length(res_array); i++) {
		result = f(result, res_array.a[i]);	
	}

	return result;
}
