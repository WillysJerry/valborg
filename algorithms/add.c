// in: [1, 2, 3, 4, 5] * 2
// out: [2, 4, 6, 8, 10]
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#include "../src/runtime/sequential/sequential.h"
#include "../src/runtime/parallel/parallel.h"
#include "../src/runtime/runtime.h"

int is_odd(int i, void* cmp) {
	return i % 2 == 1;
}

double mul2(double x, double y) {
	return x+y;
}

int main(int argc, char** argv) {
	double arr0[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
	par_array A = mk_array(arr0, 0, 4);
	init_par_env();
	par_array B = par_replicate(2.0, A.m, A.n, is_odd, NULL);
	par_array C = par_map2(mul2, A, B, NULL, NULL);
	destroy_par_env();

	for(int i = 0; i < length(C); i++) {
		printf("%.2f ", VAL(C.a[i])); 
	}

	printf("\n");
	
	return 0;
}
