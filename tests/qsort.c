#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "../src/runtime/sequential/sequential.h"
#include "../src/runtime/parallel/parallel.h"
#include "../src/runtime/runtime.h"

int leq(int i, const par_array x, void* cmp) {
	double val = *((double*)cmp);
	return IS_SOME(ELEM(x, i)) && (VAL(ELEM(x, i)) <= val);
}

int gt(int i, const par_array x, void* cmp) {
	double val = *((double*)cmp);
	return IS_SOME(ELEM(x, i)) && (VAL(ELEM(x, i)) > val);
}

double neg(double x) {
	return -x;
}

par_array quick_sort(par_array in) {
	if(par_count(in, NULL, NULL) < 2) {
		return in;
	}

	// We assume that the array is dense for now 
	// (if it is not we need to make sure that the pivot is SOME)
	double pivot = VAL(in.a[0]);

	par_array eql = par_select(in, L2G(in, 0), L2G(in, 0), NULL, NULL); 

	par_array les = par_select(in, in.m, in.n, leq, (void*) &pivot);
	par_array grt = par_select(in, in.m, in.n, gt, (void*) &pivot);
	

	par_array les_sorted = quick_sort(les);
	par_array grt_sorted = quick_sort(grt);


	par_array left = par_concat(les_sorted, eql);
	par_array finl = par_concat(left, grt_sorted);

	free(eql.a);
	free(les.a);
	free(grt.a);
	free(les_sorted.a);
	free(grt_sorted.a);
	free(left.a);

	return finl;

}

int main(int argc, char** argv) {
	par_array R; 	// Resulting array

	double arr0[] = { 4.0, 2.0, 8.0, 1.0, 6.0, 7.0, 5.0, 3.0 };
	par_array A = mk_array(arr0, 0, 7);

	init_par_env();

	R = quick_sort(A);

	for(int i = 0; i < length(R); i++) {
		if(IS_SOME(R.a[i])) {
			printf("%f ", VAL(R.a[i]));
		}
	}
	printf("\n");

	destroy_par_env();

	return 0;
}
