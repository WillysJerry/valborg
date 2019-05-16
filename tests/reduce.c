#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#include <runtime.h>
#include "benchmark.h"

#define EPSILON 0.0001
#define T1 100
#define T2 3444
#define T3 63595 
#define T4 800000
#define N_TESTS 1000
#define MAX 1000

double sum2(double x, double y) {
	return x+y;
}

double mul2(double x, double y) {
	return x*y;
}

int gt4(int i, const par_array x, void* cmp) {
	return IS_SOME(ELEM(x, i)) && (VAL(ELEM(x, i)) > 4);
}
int gt100(int i, const par_array x, void* cmp) {
	return IS_SOME(ELEM(x, i)) && (VAL(ELEM(x, i)) > 100);
}


int main(int argc, char** argv) {
	double r, r2; 	// Resulting scalar
	double t0, t1;
	double seq_t, par_t;
	
	double arr0[] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0 };
	double arr1[T1];
	double arr2[T2];
	double arr3[T3];
	double arr4[T4];

	srand(time(0));
	vb_init_par_env();

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

	printf("Initial test...\n");
	r = vb_reduce(sum2, S, NULL, NULL);
	
	r2 = 0.0;
	for(int i = 0; i < length(S); i++) {
		r2 = sum2(r2, VAL(S.a[i]));	
	}
	printf("%f %f\ndiff:%f\n", r, r2, fabs(r-r2));

	printf("#### TEST 1 ####\n");
	seq_t = 0.0;
	par_t = 0.0;
	for(int i = 0; i < N_TESTS; i++) {
		t0 = get_time_usec();
		r = vb_reduce(sum2, A, gt4, NULL);
		t1 = get_time_usec();
		par_t += get_timediff(t0, t1);

		t0 = get_time_usec();
		r2 = 0.0;
		for(int j = A.m; j < A.m + length(A); j++) {
			if(VAL(A.a[G2L(A, j)]) > 4.0)
				r2 = sum2(r2, VAL(A.a[G2L(A, j)]));
		}
		t1 = get_time_usec();
		seq_t += get_timediff(t0, t1);
		if(fabs(r - r2) > EPSILON) {
			printf("%f %f\ndiff:%f\n", r, r2, fabs(r-r2));

		}
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
		r = vb_reduce(sum2, B, gt4, NULL);
		t1 = get_time_usec();
		par_t += get_timediff(t0, t1);

		t0 = get_time_usec();
		r2 = 0.0;
		for(int j = B.m; j < B.m + length(B); j++) {
			if(VAL(B.a[G2L(B, j)]) > 4.0)
				r2 = sum2(r2, VAL(B.a[G2L(B, j)]));
		}
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
		r = vb_reduce(sum2, C, gt4, NULL);
		t1 = get_time_usec();
		par_t += get_timediff(t0, t1);

		t0 = get_time_usec();
		r2 = 0.0;
		for(int j = C.m; j < C.m + length(C); j++) {
			if(VAL(C.a[G2L(C, j)]) > 4.0)
				r2 = sum2(r2, VAL(C.a[G2L(C, j)]));
		}
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
		r = vb_reduce(sum2, D, gt100, NULL);
		t1 = get_time_usec();
		par_t += get_timediff(t0, t1);

		t0 = get_time_usec();
		r2 = 0.0;
		for(int j = D.m; j < D.m + length(D); j++) {
			if(VAL(D.a[G2L(D, j)]) > 100.0)
				r2 = sum2(r2, VAL(D.a[G2L(D, j)]));
		}
		t1 = get_time_usec();
		seq_t += get_timediff(t0, t1);
		assert(fabs(r - r2) <= EPSILON);
	}
	seq_t /= N_TESTS;
	par_t /= N_TESTS;

	log_benchmark(seq_t, par_t, T4, "+-reduce");
	//printf("%f %f\ndiff:%f\n", r, r2, fabs(r-r2));
	vb_destroy_par_env();
	return 0;
}
