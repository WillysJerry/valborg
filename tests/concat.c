#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#include <runtime.h>
#include "benchmark.h"

#define EPSILON 0.0001
#define T1 304200
#define T2 624410 
#define T3 63595 
#define T4 800000
#define N_TESTS 100
#define MAX 10


int main(int argc, char** argv) {
	int k;
	double t0, t1;
	double seq_t, par_t;

	par_array R1, R2;
	double arr1[T1];
	double arr2[T2];

	for(int i = 0; i < T1; i++) {
		arr1[i] = (double)rand()/(double)(RAND_MAX / MAX);
	}
	for(int i = 0; i < T2; i++) {
		arr2[i] = (double)rand()/(double)(RAND_MAX / MAX);
	}

	const par_array A = mk_array(arr1, 0, T1-1);
	const par_array B = mk_array(arr2, 0, T2-1);

	vb_init_par_env();

	printf("#### TEST 1 ####\n");
	seq_t = 0.0;
	par_t = 0.0;
	for(int i = 0; i < N_TESTS; i++) {
		t0 = get_time_usec();
		R1 = vb_concat(A, B);
		t1 = get_time_usec();
		par_t += get_timediff(t0, t1);

		t0 = get_time_usec();
		int j;
		R2 = mk_array(NULL, A.m, A.m + length(A) + length(B) - 1);
		for(j = A.m; j < A.m + length(A); j++) {
			R2.a[G2L(R2, j)] = A.a[G2L(A, j)];
		}
		for(; j < A.m + length(A) + length(B); j++) {
			R2.a[G2L(R2, j)] = B.a[G2L(B, j - length(A))];
		}
		t1 = get_time_usec();
		seq_t += get_timediff(t0, t1);

		for(k = 0; k < length(A); k++) {
			assert(fabs(VAL(R1.a[k]) - VAL(A.a[k])) <= EPSILON);
			assert(fabs(VAL(R1.a[k]) - VAL(R2.a[k])) <= EPSILON);
		}
		for(k += 1; k < length(A) + length(B); k++) {
			assert(fabs(VAL(R1.a[k]) - VAL(B.a[k - length(A)])) <= EPSILON);
			assert(fabs(VAL(R1.a[k]) - VAL(R2.a[k])) <= EPSILON);
		}

		free(R1.a);
		free(R2.a);
	}
	seq_t /= N_TESTS;
	par_t /= N_TESTS;

	log_benchmark(seq_t, par_t, length(A) + length(B), "concat");

	vb_destroy_par_env();
	return 0;
}
