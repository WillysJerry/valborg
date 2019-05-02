#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#include "../src/runtime/sequential.h"
#include "../src/runtime/parallel.h"
#include "../src/runtime/runtime.h"
#include "benchmark.h"

#define EPSILON 0.0001
#define T1 100
#define T2 3444
#define T3 63595 
#define T4 800000
#define N_TESTS 100
#define MAX 1000

double sum2(double x, double y) {
	return x+y;
}

double mul2(double x, double y) {
	return x*y;
}


int main(int argc, char** argv) {
	double r, r2; 	// Resulting scalar
	double t0, t1;
	double seq_t, par_t;
	
	double arr1[T1];
	double arr2[T2];
	double arr3[T3];
	double arr4[T4];

	srand(time(0));

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
	const par_array A = mk_array(arr1, 0, T1-1);
	const par_array B = mk_array(arr2, 0, T2-1);
	const par_array C = mk_array(arr3, 0, T3-1);
	const par_array D = mk_array(arr4, 0, T4-1);

	printf("#### TEST 1 ####\n");
	seq_t = 0.0;
	par_t = 0.0;
	for(int i = 0; i < N_TESTS; i++) {
		t0 = get_time_usec();
		r = par_reduce(sum2, A);
		t1 = get_time_usec();
		par_t += get_timediff(t0, t1);

		t0 = get_time_usec();
		r2 = seq_reduce(sum2, A);
		t1 = get_time_usec();
		seq_t += get_timediff(t0, t1);
		assert(fabs(r - r2) <= EPSILON);
	}
	seq_t /= N_TESTS;
	par_t /= N_TESTS;

	log_benchmark(seq_t, par_t, length(A), "+-reduce");
	//printf("%f %f\ndiff:%f\n", r, r2, fabs(r-r2));

	printf("#### TEST 2 ####\n");
	seq_t = 0.0;
	par_t = 0.0;
	for(int i = 0; i < N_TESTS; i++) {
		t0 = get_time_usec();
		r = par_reduce(sum2, B);
		t1 = get_time_usec();
		par_t += get_timediff(t0, t1);

		t0 = get_time_usec();
		r2 = seq_reduce(sum2, B);
		t1 = get_time_usec();
		seq_t += get_timediff(t0, t1);
		assert(fabs(r - r2) <= EPSILON);
	}
	seq_t /= N_TESTS;
	par_t /= N_TESTS;

	log_benchmark(seq_t, par_t, T2, "+-reduce");
	//printf("%f %f\ndiff:%f\n", r, r2, fabs(r-r2));

	printf("#### TEST 3 ####\n");
	seq_t = 0.0;
	par_t = 0.0;
	for(int i = 0; i < N_TESTS; i++) {
		t0 = get_time_usec();
		r = par_reduce(sum2, C);
		t1 = get_time_usec();
		par_t += get_timediff(t0, t1);

		t0 = get_time_usec();
		r2 = seq_reduce(sum2, C);
		t1 = get_time_usec();
		seq_t += get_timediff(t0, t1);
		assert(fabs(r - r2) <= EPSILON);
	}
	seq_t /= N_TESTS;
	par_t /= N_TESTS;

	log_benchmark(seq_t, par_t, T3, "+-reduce");
	//printf("%f %f\ndiff:%f\n", r, r2, fabs(r-r2));

	printf("#### TEST 4 ####\n");
	seq_t = 0.0;
	par_t = 0.0;
	for(int i = 0; i < N_TESTS; i++) {
		t0 = get_time_usec();
		r = par_reduce(sum2, D);
		t1 = get_time_usec();
		par_t += get_timediff(t0, t1);

		t0 = get_time_usec();
		r2 = seq_reduce(sum2, D);
		t1 = get_time_usec();
		seq_t += get_timediff(t0, t1);
		assert(fabs(r - r2) <= EPSILON);
	}
	seq_t /= N_TESTS;
	par_t /= N_TESTS;

	log_benchmark(seq_t, par_t, T4, "+-reduce");
	//printf("%f %f\ndiff:%f\n", r, r2, fabs(r-r2));
	return 0;
}
