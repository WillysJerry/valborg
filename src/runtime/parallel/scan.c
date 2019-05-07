#include "../parallel.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <math.h>

void scan_thrd(int i, par_array* in, void* out, void* f, void* p, void* args) {
	par_array* output_arr = (par_array*)out;
	int k = (int)args;

	int (*pred)(int i) =
		(int (*)(int)) p;
	double (*func)(double x, double y) =
		(double (*)(double x, double y)) f;

	double x = par_array_get(*in, i - (0x1 << k)); 
	double y = par_array_get(*in, i); 


	par_array_set(*output_arr, i, func(x, y));
}

par_array par_scan(double (*f)(double x, double y), const par_array a, int (*p)(int i)) {
	int len, until;
	par_array curr = mk_array(a.a, a.m, a.n);
	par_array output;

	until = ceil(log2(length(a)));		// Set loop limit
	for(int i = 0; i < until; i++) {
		output = mk_array(curr.a, a.m, a.n);
		execute_in_parallel(scan_thrd, &curr, &output, a.m + (0x1 << i), a.n, (void*)f, (void*)p, (void*)i);
		free(curr.a);
		curr = output;	
	}

	return output;
}
