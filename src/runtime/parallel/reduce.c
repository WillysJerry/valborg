#include "../parallel.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <math.h>

// Evaluates one level of the reduction tree
void reduce_thrd(distribution dist, int id, par_array* out, void* f, void* p, void* args) {
	int i;
	double res = 0.0;
	int size = dist.b_size[id];

	maybe* A = dist.arrs[0].a + dist.blocks[id];
	int arrbase = dist.m + dist.blocks[id];
	
	// Cast p pointer to predicate function
	int (*pred)(int i) =
		(int (*)(int)) p;


	for(i = 0; i < size; i++) {
		if(IS_SOME(A[i])) {
			if(pred == NULL || pred(i + arrbase)) {
				res = VAL(A[i]); 
				break;
			}
		}
	}

	// Handle base cases
	if(size == 0 || i == size) {
		out->a[id] = NONE;
		return;
	}
	else if(size == 1) {
		out->a[id] = SOME(res);
		return;
	}

	// Cast void pointers to functions, this is not very readable
	double (*func)(double x, double y) =
		(double (*)(double x, double y)) f;

	for(i += 1; i < size; i++) {
		if(IS_SOME(A[i])) {
			if(pred == NULL || pred(i + arrbase)) {
				res = func(res, VAL(A[i]));	
			}
		}
	}

	out->a[id] = SOME(res);
}

double par_reduce(double (*f)(double x, double y), const par_array a, int (*p)(int i)) {
	distribution dist;
	double result = 0.0;

	par_array res_array;

	dist = distribute(&a, 1, DISTRIBUTION_STRICT);

	res_array = execute_in_parallel(reduce_thrd, dist, 0, 3, (void*)f, (void*)p, NULL);

	result = VAL(res_array.a[0]);
	for(int i = 1; i < NUM_THREADS; i++) {
		result = f(result, VAL(res_array.a[i]));
	}

	free_distribution(dist);
	free(res_array.a);
	return result;
}
