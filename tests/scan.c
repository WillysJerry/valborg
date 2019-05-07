#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "../src/runtime/sequential.h"
#include "../src/runtime/parallel.h"
#include "../src/runtime/runtime.h"

#include "benchmark.h"

#define EPSILON 0.0001
#define T1 100000
#define T2 3444
#define T3 63595 
#define T4 800000
#define N_TESTS 100
#define MAX 1

double sum2(double x, double y) {
	return x+y;
}

int main(int argc, char** argv) {
	par_array R; 	// Resulting array
	par_array R2; 	// Resulting array
	
	double t0, t1;
	double seq_t, par_t;

	double arr0[] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0 };
	double arr1[T1];
	double arr2[T2];
	double arr3[T3];
	double arr4[T4];
	
	srand(time(0));
	init_par_env();

	// "Never repeat yourself"
	for(int i = 0; i < T1; i++) {
		arr1[i] = (double)rand()/(double)(RAND_MAX / MAX);
	}
	for(int i = 0; i < T2; i++) {
		arr2[i] = (double)rand()/(double)(RAND_MAX / MAX);
	}
	for(int i = 0; i < T3; i++) {
		arr3[i] = (double)rand()/(double)(RAND_MAX / MAX);
	}
	for(int i = 0; i < T4; i++) {
		arr4[i] = (double)rand()/(double)(RAND_MAX / MAX);
	}
	const par_array S = mk_array(arr0, 3, 8);
	const par_array A = mk_array(arr1, 0, T1-1);
	const par_array B = mk_array(arr2, 0, T2-1);
	const par_array C = mk_array(arr3, 0, T3-1);
	const par_array D = mk_array(arr4, 0, T4-1);

	printf("Initial test\n");
	seq_t = 0.0;
	par_t = 0.0;
	for(int i = 0; i < 1; i++) {
		t0 = get_time_usec();
		R = par_scan(sum2, S, NULL);
		t1 = get_time_usec();
		par_t += get_timediff(t0, t1);

		t0 = get_time_usec();
		R2 = seq_scan(sum2, S, NULL);
		t1 = get_time_usec();
		seq_t += get_timediff(t0, t1);

		for(int j = 0; j < 6; j++) {
			printf("%f    %f\n", R.a[j], R2.a[j]);
			/*
			if(fabs(R.a[j] - R2.a[j]) > EPSILON) {
				printf("%f %f\ndiff:%f\n", R.a[j], R2.a[j], fabs(R.a[j]-R2.a[j]));
				assert(fabs(R.a[j] - R2.a[j]) > EPSILON);

			}*/
		}
	}
	seq_t /= N_TESTS;
	par_t /= N_TESTS;

	log_benchmark(seq_t, par_t, 6, "+-scan");

	printf("#### TEST 1 ####\n");
	seq_t = 0.0;
	par_t = 0.0;
	for(int i = 0; i < N_TESTS; i++) {
		t0 = get_time_usec();
		R = par_scan(sum2, A, NULL);
		t1 = get_time_usec();
		par_t += get_timediff(t0, t1);

		t0 = get_time_usec();
		R2 = seq_scan(sum2, A, NULL);
		t1 = get_time_usec();
		seq_t += get_timediff(t0, t1);

		for(int j = 0; j < T1; j++) {
			if(fabs(R.a[j] - R2.a[j]) > EPSILON) {
				printf("%f %f\ndiff:%f\n", R.a[j], R2.a[j], fabs(R.a[j]-R2.a[j]));
				assert(fabs(R.a[j] - R2.a[j]) > EPSILON);

			}
		}
	}
	seq_t /= N_TESTS;
	par_t /= N_TESTS;

	log_benchmark(seq_t, par_t, length(A), "+-scan");

	printf("\n");
	destroy_par_env();
	return 0;
}
