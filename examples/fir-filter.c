// FIR-filter example 
//
// Input: 
// 	X = some input time series (parallel array)
// 	W = some array of weights 
// Do: 	
//	Y = replicate(0.0, X.m, length(X) - length(W))
// 	forall i in W do
// 		forall j in Y in parallel do
//	 		Y[j] = Y[j] + W[i] * X[j+1]

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
int slide(int i, void* arg) {
	int s = *(int*)arg;
	return i + s;
}
int main(int argc, char** argv) {
	double t0, t1;
	double tot_t, avg_t;
	// Input time series
	double arr0[SIZE];
	par_array A = mk_array(arr0, 0, SIZE-1);
	for(int i = 0; i < SIZE; i++) {
		A.a[i] = SOME((double)i);
	}

	// Non-parallel array of k weights
	double weights[] = { 0.3, 0.5, 1.0 };
	int k = 3;

	par_array Y;
	double sum;


	tot_t = 0.0;
	for(int w = 0; w < N_TESTS; w++) {
		t0 = get_time_usec();
		Y = mk_array(NULL, A.m, A.n - k + 1);
		for(int j = 0; j < length(Y); j++) {
			sum = 0.0;
			for(int i = 0; i < k; i++) {
				if(IS_SOME(A.a[i + j - 1]))
					sum += weights[i] * VAL(A.a[i + j - 1]);
			}
			Y.a[j] = SOME(sum);
		}
		t1 = get_time_usec();
		tot_t += get_timediff(t0,t1);
		free(Y.a);
	}

	avg_t = tot_t / (double)N_TESTS;

	printf("Sequential: %.1f\n", avg_t); 

	tot_t = 0.0;
	vb_init_par_env();	
		for(int w = 0; w < N_TESTS; w++) {
			t0 = get_time_usec();
			// For each weight...
			Y = vb_replicate(0.0, A.m, A.n - k + 1, NULL, NULL);
			par_array prev;
			for(int i = 0; i < k; i++) {
				par_array W_i = vb_replicate(weights[i], A.m, A.n, NULL, NULL);
				par_array X_i = vb_get(A, slide, (void*)&i, NULL, NULL);
				par_array B = vb_map2(mul2, W_i, X_i, NULL, NULL);
				prev = Y;
				Y = vb_map2(sum2, B, Y, NULL, NULL);

				free(W_i.a);
				free(X_i.a);
				free(B.a);
				free(prev.a);
			}
			t1 = get_time_usec();
			tot_t += get_timediff(t0,t1);
			free(Y.a);
		}
	vb_destroy_par_env();

	avg_t = tot_t / (double)N_TESTS;

	printf("Parallel: %.1f\n", avg_t); 
	return 0;

	for(int i = 0; i < length(Y); i++) {
		printf("%.2f ", VAL(Y.a[i])); 
	}

	printf("\n");

	return 0;
}
