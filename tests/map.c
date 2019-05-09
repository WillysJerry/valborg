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
#define T1 800000
#define T2 80000 
#define T3 80000 
#define T4 800
#define N_TESTS 100
#define MAX 10

double square(double x) {
	return x*x;
}
double neg(double x) {
	return -x;
}
double sum2(double x, double y) {
	return x+y;
}
double mul3(double x, double y, double z) {
	return x*y*z;
}

int x_lt2(int i, const par_array x, const par_array y) {
	return IS_SOME(ELEM(x, i)) && (VAL(ELEM(x, i)) < 2.0);
}

int is_neg(int i, const par_array x) {
	return IS_SOME(ELEM(x, i)) && (VAL(ELEM(x, i)) < 0.0);
}

int main(int argc, char** argv) {
	par_array R1, R2; 	// Resulting array
	double t0, t1;
	double seq_t, par_t;
	
	double arr1[] = { 	1.0, 	2.0, 	3.0, 	4.0,	5.0,	6.0 };
	double arr2[] = { 	3.0, 	4.0, 	5.0, 	6.0, 	7.0, 	8.0 };
	double arr3[] = { 	-1.0,	-2.0, 	-3.0, 	-4.0, 	-5.0, 	-6.0 };
	double arr4[] = { 	-1.0,	2.0, 	-3.0, 	-4.0, 	5.0, 	-6.0 };
	
	double arr5[T1];
	double arr6[T2];
	double arr7[T3];
	srand(time(0));

	for(int i = 0; i < T1; i++) {
		arr5[i] = (double)rand()/(double)(RAND_MAX / MAX);
	}
	for(int i = 0; i < T2; i++) {
		arr6[i] = (double)rand()/(double)(RAND_MAX / MAX);
	}
	for(int i = 0; i < T3; i++) {
		arr7[i] = (double)rand()/(double)(RAND_MAX / MAX);
	}

	const par_array A = mk_array(arr1, 3, 8);
	const par_array B = mk_array(arr2, 0, 5);
	const par_array C = mk_array(arr3, 0, 5);
	const par_array D = mk_array(arr4, 0, 5);

	const par_array E = mk_array(arr4, 0, T1-1);
	const par_array F = mk_array(arr5, 0, T2-1);
	const par_array G = mk_array(arr6, 0, T3-1);


	init_par_env();

	R1 = par_map1(square, A, NULL);
	printf("Square A:\n");
	for(int i = 0; i < length(R1); i++) {
		printf("%f ", VAL(R1.a[i]));
	}
	free(R1.a);

	printf("\nSum A B:\n");
	R1 = par_map2(sum2, A, B, x_lt2);
	for(int i = 0; i < length(R1); i++) {
		printf("%f ", VAL(R1.a[i]));
	}
	free(R1.a);

	printf("\nMul A B C:\n");
	R1 = par_map3(mul3, A, B, C, NULL);
	for(int i = 0; i < length(R1); i++) {
		printf("%f ", VAL(R1.a[i]));
	}
	free(R1.a);

	R1 = par_map1(neg, D, is_neg);
	printf("Abs D:\n");
	for(int i = 0; i < length(R1); i++) {
		printf("%f ", VAL(R1.a[i]));
	}
	free(R1.a);

	for(int i = 0; i < N_TESTS; i++) {
		t0 = get_time_usec();
		R1 = par_map1(square, E, NULL);
		t1 = get_time_usec();
		par_t += get_timediff(t0, t1);

		t0 = get_time_usec();
		R2 = seq_map1(square, E, NULL);
		t1 = get_time_usec();
		seq_t += get_timediff(t0, t1);

		for(int j = 0; j < T1; j++) {
			assert( abs(VAL(R1.a[j]) - square(VAL(E.a[j]))) <= EPSILON);
		}

		free(R1.a);
		free(R2.a);
	}
	seq_t /= N_TESTS;
	par_t /= N_TESTS;

	log_benchmark(seq_t, par_t, T1, "map1");

	for(int i = 0; i < N_TESTS; i++) {
		t0 = get_time_usec();
		R1 = par_map2(sum2, F, F, NULL);
		t1 = get_time_usec();
		par_t += get_timediff(t0, t1);

		t0 = get_time_usec();
		R2 = seq_map2(sum2, F, F, NULL);
		t1 = get_time_usec();
		seq_t += get_timediff(t0, t1);

		for(int j = 0; j < T2; j++) {
			assert( abs(VAL(R1.a[j]) - sum2(VAL(F.a[j]), VAL(F.a[j]))) <= EPSILON);
		}

		free(R1.a);
		free(R2.a);
	}
	seq_t /= N_TESTS;
	par_t /= N_TESTS;

	log_benchmark(seq_t, par_t, T2, "map2");

	for(int i = 0; i < N_TESTS; i++) {
		t0 = get_time_usec();
		R1 = par_map3(mul3, G, G, G, NULL);
		t1 = get_time_usec();
		par_t += get_timediff(t0, t1);

		t0 = get_time_usec();
		R2 = seq_map3(mul3, G, G, G, NULL);
		t1 = get_time_usec();
		seq_t += get_timediff(t0, t1);

		for(int j = 0; j < T3; j++) {
			assert( abs(VAL(R1.a[j]) - mul3(VAL(G.a[j]), VAL(G.a[j]), VAL(G.a[j]))) <= EPSILON);
		}

		free(R1.a);
		free(R2.a);
	}
	seq_t /= N_TESTS;
	par_t /= N_TESTS;

	log_benchmark(seq_t, par_t, T3, "map3");
	printf("\n");
	destroy_par_env();
	return 0;
}
