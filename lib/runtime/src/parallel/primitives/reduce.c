#include "../parallel.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <math.h>

// Evaluates one level of the reduction tree
void reduce_thrd(distribution dist, int id, par_array* out, void* f, void* p, void* args, void* cmp) {
	int i;
	double res;
	int size = dist.b_size[id];

	par_array* A = dist.arrs;
	maybe* A_values = A->a + dist.blocks[id];
	int arrbase = dist.m + dist.blocks[id];
	
	// Cast p pointer to predicate function
	int (*pred)(int i, const par_array x, void* cmp) =
		(int (*)(int, const par_array, void*)) p;


	/*for(i = 0; i < size; i++) {
		if(IS_SOME(A_values[i]) && SATISFIES(pred, i + arrbase, *A, cmp)) {
			res = VAL(A_values[i]); 
			break;
		}
	}*/


	// Handle base cases
	/*if(size == 0 || i == size) {
		out->a[id] = SOME(initial);
		return;
	}*/

	// Cast void pointers to functions, this is not very readable
	double (*func)(double x, double y) =
		(double (*)(double x, double y)) f;

	res = *(double*)args;
	for(i = 0; i < size; i++) {
		if(IS_SOME(A_values[i]) && SATISFIES(pred, i + arrbase, *A, cmp)) {
			res = func(res, VAL(A_values[i]));	
		}
	}

	out->a[id] = SOME(res);
}

double vb_reduce(double (*f)(double x, double y), double v, const par_array a, int (*p)(int i, par_array x, void* cmp), void* cmp) {
	distribution dist;
	double result;

	par_array res_array = mk_array(NULL, a.m, a.m + NUM_THREADS - 1);

	dist = distribute(&a, 1, a.m, a.n);

	execute_in_parallel(reduce_thrd, dist, &res_array, (void*)f, (void*)p, (void*)&v, cmp);

	result = VAL(res_array.a[0]);
	for(int i = 1; i < NUM_THREADS; i++) {
		result = f(result, VAL(res_array.a[i]));
	}

	free_distribution(dist);
	free(res_array.a);
	return result;
}
