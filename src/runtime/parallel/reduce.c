#include "../parallel.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <math.h>
//void (*work)(int i, par_array* input, void* output, void* f, void* p, void* args)
// Evaluates one level of the reduction tree
void reduce_thrd(int i, par_array* in, void* out, void* f, void* p, void* args) {
	int (*pred)(int i) =
		(int (*)(int)) p;
	double (*func)(double x, double y) =
		(double (*)(double x, double y)) f;

	double x, y;
	par_array* p_out = (par_array*)out;
	int base = global_to_local(*in, i);

	int x_i = local_to_global(*in, 2 * base);
	int y_i = x_i + 1;
	if(y_i > in->n) {
		par_array_set(*p_out, i, par_array_get(*in, x_i));
		return;

	}

	x = par_array_get(*in, local_to_global(*in, 2 * base)); 
	y = par_array_get(*in, local_to_global(*in, 2 * base + 1)); 
	par_array_set(*p_out, i, func(x,y));
}

double par_reduce(double (*f)(double x, double y), const par_array a, int (*p)(int i)) {
	int len, until;
	double result;

	par_array work_arrs[2];
	work_arrs[0] = mk_array(a.a, a.m, a.n);
	work_arrs[1] = mk_array(NULL, a.m, a.n);
	par_array* in = work_arrs;
	par_array* out;

	len = length(a);

	until = ceil( log2(len) );	// Set loop limit

	for(int i = 0; i < until; i++) {
		out = work_arrs + ((i+1) % 2);	// Alternate between work arrays
		len = ceil(len / 2.0);		// 

		execute_in_parallel(
				reduce_thrd, 			// Function to be executed for each element (in parallel)
				in, 				// Input array
				out,				// Output array 
				in->m, 				// Starting index
				in->m + len - 1, 		// Last index
				(void*)f, (void*)p, NULL);	// Args passed to element function

		out->n = in->m + len - 1;
		in = out;	// Input of the next iteration should be the result of the
				// current iteration's (parallel) computation.
	}

	result = out->a[0];
	free(work_arrs[0].a);
	free(work_arrs[1].a);

	return result;
}
