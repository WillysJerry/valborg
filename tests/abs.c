#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#include <runtime.h>
#include "benchmark.h"

#define EPSILON 0.01
#define T1 950000
#define T2 62441 
#define T3 63595 
#define T4 800000
#define N_TESTS 100
#define MAX 10

double neg_if_neg(double x) {
	return x < 0.0 ? -x : x;
}

double par_abs(par_array in) {
	double t0, t1;
	double par_t;
	par_array R1;
	par_array S_absed;

	par_t = 0.0;
	for(int i = 0; i < N_TESTS; i++) {
		t0 = get_time_usec();
		R1 = vb_map1(neg_if_neg, in, NULL, NULL);
		t1 = get_time_usec();
		par_t += get_timediff(t0, t1);

		for(int i = 0; i < length(R1); i++) {
			assert(VAL(R1.a[i]) > (0.0 - EPSILON));
		}

		free(R1.a);
	}
	par_t /= N_TESTS;
	return par_t;


}

double seq_abs(par_array in) {
	par_array R; 	
	double t0, t1;
	double seq_t = 0.0;
	for(int i = 0; i < N_TESTS; i++) {

		t0 = get_time_usec();
		R = mk_array(NULL, in.m, in.n);
		for(int j = 0; j < T1; j++) {
			if(IS_SOME(in.a[j]) && (VAL(in.a[j]) < (0.0 - EPSILON)) ) {
				R.a[j] = SOME( -VAL(in.a[j]) );
			} else {
				R.a[j] = in.a[j];
			}
		}
		t1 = get_time_usec();
		seq_t += get_timediff(t0, t1);

		for(int i = 0; i < length(R); i++) {
			assert(VAL(R.a[i]) > (0.0 - EPSILON));
		}

		free(R.a);
	}
	seq_t /= N_TESTS;
	return seq_t;

}

int main(int argc, char** argv) {

	double arr0[T1];

	for(unsigned long long i = 0; i < T1; i++) {
		arr0[i] = (double)rand()/(double)( (RAND_MAX / MAX) ) - (double)(MAX / 2.0);
	}

	par_array A = mk_array(arr0, 0, T1-1);
	double seq_t, par_t;

	seq_t = seq_abs(A);
	vb_init_par_env();
		par_t = par_abs(A);
	vb_destroy_par_env();
	log_benchmark(seq_t, par_t, T1, "abs");

	return 0;
}
