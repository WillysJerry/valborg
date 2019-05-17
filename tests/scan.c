#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#include <runtime.h>
#include "benchmark.h"

#define EPSILON 0.0001
#define T1 100000 
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

int gt4(int i, const par_array x, void* cmp) {
	return IS_SOME(ELEM(x, i)) && (VAL(ELEM(x, i)) > 4);
}
int gt100(int i, const par_array x, void* cmp) {
	return IS_SOME(ELEM(x, i)) && (VAL(ELEM(x, i)) > 100);
}


int main(int argc, char** argv) {
	par_array R1, R2; 	// Resulting scalar
	double t0, t1;
	double seq_t, par_t;
	
	double arr0[] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0 };
	double arr1[T1];
	double arr2[T2];
	double arr3[T3];
	double arr4[T4];

	int k;

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
	R1 = vb_scan(sum2, 0.0, S, NULL, NULL);

	R2 = mk_array(NULL, S.m, S.n);
	double acc = VAL(ELEM(S, S.m));
	R2.a[G2L(R2, S.m)] = SOME(acc);
	for(int i = R2.m + 1; i < R2.m + length(R2); i++) {
		acc = sum2(acc, VAL(S.a[G2L(S, i)]));
		R2.a[G2L(R2, i)] = SOME(acc);
	}

	for(int i = 0; i < length(S); i++) {
		printf("P:%.1f S:%.1f\n", VAL(R1.a[i]), VAL(R2.a[i]));

	}
	free(R1.a);
	free(R2.a);

	printf("#### TEST 1 ####\n");
	seq_t = 0.0;
	par_t = 0.0;
	for(int i = 0; i < N_TESTS; i++) {
		t0 = get_time_usec();
		R1 = vb_scan(sum2, 0.0, A, NULL, NULL);
		t1 = get_time_usec();
		par_t += get_timediff(t0, t1);

		t0 = get_time_usec();
		R2 = mk_array(NULL, A.m, A.n);
		acc = VAL(ELEM(A, A.m));
		R2.a[G2L(R2, A.m)] = SOME(acc);
		for(int j = R2.m + 1; j < R2.m + length(R2); j++) {
			acc = sum2(acc, VAL(A.a[G2L(A, j)]));
			R2.a[G2L(R2, j)] = SOME(acc);
		}
		t1 = get_time_usec();
		seq_t += get_timediff(t0, t1);

		for(k = 0; k < length(A); k++) {
			assert(fabs(VAL(R1.a[k]) - VAL(R2.a[k])) <= EPSILON);
		}

		free(R1.a);
		free(R2.a);
	}
	seq_t /= N_TESTS;
	par_t /= N_TESTS;

	log_benchmark(seq_t, par_t, length(A), "+-scan");
	//printf("%f %f\ndiff:%f\n", r, r2, fabs(r-r2));

	printf("#### TEST 2 ####\n");
	seq_t = 0.0;
	par_t = 0.0;
	for(int i = 0; i < N_TESTS; i++) {
		t0 = get_time_usec();
		R1 = vb_scan(sum2, 0.0, B, NULL, NULL);
		t1 = get_time_usec();
		par_t += get_timediff(t0, t1);

		t0 = get_time_usec();
		R2 = mk_array(NULL, B.m, B.n);
		acc = VAL(ELEM(B, B.m));
		R2.a[G2L(R2, B.m)] = SOME(acc);
		for(int j = R2.m + 1; j < R2.m + length(R2); j++) {
			acc = sum2(acc, VAL(B.a[G2L(B, j)]));
			R2.a[G2L(R2, j)] = SOME(acc);
		}
		t1 = get_time_usec();
		seq_t += get_timediff(t0, t1);

		free(R1.a);
		free(R2.a);
	}
	seq_t /= N_TESTS;
	par_t /= N_TESTS;

	log_benchmark(seq_t, par_t, T2, "+-scan");
	//printf("%f %f\ndiff:%f\n", r, r2, fabs(r-r2));

	printf("#### TEST 3 ####\n");
	seq_t = 0.0;
	par_t = 0.0;
	for(int i = 0; i < N_TESTS; i++) {
		t0 = get_time_usec();
		R1 = vb_scan(sum2, 0.0, C, gt4, NULL);
		t1 = get_time_usec();
		par_t += get_timediff(t0, t1);

		t0 = get_time_usec();
		R2 = mk_array(NULL, C.m, C.n);
		acc = VAL(ELEM(C, C.m));
		R2.a[G2L(R2, C.m)] = SOME(acc);
		for(int j = R2.m + 1; j < R2.m + length(R2); j++) {
			acc = sum2(acc, VAL(C.a[G2L(C, j)]));
			R2.a[G2L(R2, j)] = SOME(acc);
		}
		t1 = get_time_usec();
		seq_t += get_timediff(t0, t1);

		free(R1.a);
		free(R2.a);
	}
	seq_t /= N_TESTS;
	par_t /= N_TESTS;

	log_benchmark(seq_t, par_t, T3, "+-scan");
	//printf("%f %f\ndiff:%f\n", r, r2, fabs(r-r2));

	printf("#### TEST 4 ####\n");
	seq_t = 0.0;
	par_t = 0.0;
	for(int i = 0; i < N_TESTS; i++) {
		t0 = get_time_usec();
		R1 = vb_scan(sum2, 0.0, D, NULL, NULL);
		t1 = get_time_usec();
		par_t += get_timediff(t0, t1);

		t0 = get_time_usec();
		R2 = mk_array(NULL, D.m, D.n);
		acc = VAL(ELEM(D, D.m));
		R2.a[G2L(R2, D.m)] = SOME(acc);
		for(int j = R2.m + 1; j < R2.m + length(R2); j++) {
			acc = sum2(acc, VAL(D.a[G2L(D, j)]));
			R2.a[G2L(R2, j)] = SOME(acc);
		}
		t1 = get_time_usec();
		seq_t += get_timediff(t0, t1);

		free(R1.a);
		free(R2.a);
	}
	seq_t /= N_TESTS;
	par_t /= N_TESTS;

	log_benchmark(seq_t, par_t, T4, "+-scan");
	//printf("%f %f\ndiff:%f\n", r, r2, fabs(r-r2));
	vb_destroy_par_env();
	return 0;
}
