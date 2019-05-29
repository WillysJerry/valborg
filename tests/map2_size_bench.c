#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#include <runtime.h>
#include "benchmark.h"

#define EXPONENT 7
#define EPSILON 0.01
#define N_TESTS 100
#define MAX 10

double sum2(double x, double y) {
	return x+y;
}

int main(int argc, char** argv) {

	double t0, t1;
	double tot_t, avg_t;

	size_t arr_length = (size_t)pow(10.0, EXPONENT) + 1;

	double* arr0 = (double*)calloc(arr_length, sizeof(double));
	if(arr0 == NULL) {
		fprintf(stderr, "Failed to allocate memory for test array!\n");
		exit(1);
	}

	double s;
	int cnt;

	for(int i = 0; i < arr_length; i++) {
		arr0[i] = (double)rand()/(double)( (RAND_MAX / MAX) ) - (double)(MAX / 2.0);
	}

	par_array A;
	par_array R;

	vb_init_par_env();

	for(int i = 0; i < EXPONENT + 1; i++) {
		A = mk_array(arr0, 0, (int)pow(10.0, i) - 1);

		tot_t = 0.0;
		for(int j = 0; j < N_TESTS; j++) {
			t0 = get_time_usec();
			R = vb_map2(sum2, A, A, NULL, NULL);
			t1 = get_time_usec();
			tot_t += get_timediff(t0, t1);
			free(R.a);
		}
		free(A.a);

		avg_t = tot_t / N_TESTS;
		printf("[%d]: %.1f\n", (int)pow(10.0, i), avg_t);
	}

	vb_destroy_par_env();
	return 0;
}
