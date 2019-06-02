// Vector dot product 
//
// Input: 
// 	A = [1.0, 2.0, 3.0, 4.0, 5.0]
// 	B = [6.0, 7.0, 8.0, 9.0, 10.0]
// Do: 	
// 	dot(A, B)
// Alternatively:
// 	forall k do
// 		C[k] = A[k] * B[k]
// 	r = reduce(C) 
// Expected output: 
// 	130.0	

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#include <runtime.h>

#include "benchmark.h"

#define N_TESTS 100
#define SIZE 1000000

double mul2(double x, double y) {
	return x * y;
}

double sum2(double x, double y) {
	return x + y;
}
int main(int argc, char** argv) {
	double t0, t1;
	double tot_t, avg_t;

	float res;

	par_array A = mk_array(NULL, 0, SIZE-1);
	par_array B = mk_array(NULL, 0, SIZE-1);

	for(int i = 0; i < SIZE; i++) {
		A.a[i] = SOME((double)i);
		B.a[i] = SOME((double)i);
	}

	tot_t = 0.0;
	
	for(int j = 0; j < N_TESTS; j++) {
		t0 = get_time_usec();
		res = 0.0;
		for(int i = 0; i < length(A); i++) {
			if(IS_SOME(A.a[i]) && IS_SOME(B.a[i]))
				res += VAL(A.a[i]) * VAL(B.a[i]);
		}
		t1 = get_time_usec();
		tot_t += get_timediff(t0,t1);
	}
	avg_t = tot_t / (double)N_TESTS;
	printf("Sequential: %.1f\n", avg_t); 

	tot_t = 0.0;
	vb_init_par_env();	
	for(int j = 0; j < N_TESTS; j++) {
		t0 = get_time_usec();
		// Multiply A and B element-wise
		par_array C = vb_map2( mul2, A, B, NULL, NULL );
		// Reduce the result of the element-wise multiplication to get the final dot product
		res = vb_reduce(sum2, 0.0, C, NULL, NULL);
		t1 = get_time_usec();
		tot_t += get_timediff(t0,t1);

		free(C.a);
	}
	vb_destroy_par_env();

	avg_t = tot_t / (double)N_TESTS;
	printf("Parallel: %.1f\n", avg_t); 
	return 0;
}
