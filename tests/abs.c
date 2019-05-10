#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#include "../src/runtime/sequential.h"
#include "../src/runtime/parallel.h"
#include "../src/runtime/runtime.h"
#include "benchmark.h"

#define EPSILON 0.01
#define T1 950000
#define T2 62441 
#define T3 63595 
#define T4 800000
#define N_TESTS 100
#define MAX 10

int lt0(int i, const par_array x, void* cmp) {
	return IS_SOME(ELEM(x, i)) && (VAL(ELEM(x, i)) < 0.0);
}

double neg(double x) {
	return -x;
}

int rhs_is_some(int i, const par_array lhs, const par_array rhs, void* cmp) {
	return IS_SOME(ELEM(rhs, i));

}

double par_abs(par_array in) {
	double t0, t1;
	double seq_t, par_t;
	par_array R1, R2; 	// Resulting array
	par_array P_absed, S_absed;

	seq_t = 0.0;
	par_t = 0.0;
	for(int i = 0; i < N_TESTS; i++) {
		t0 = get_time_usec();
		P_absed = par_map1(neg, in, lt0, NULL);
		R1 = par_asn(in, P_absed, rhs_is_some, NULL);
		t1 = get_time_usec();
		par_t += get_timediff(t0, t1);

		/*t0 = get_time_usec();
		S_absed = par_map1(neg, in, lt0, NULL);
		R2 = seq_asn(in, S_absed, rhs_is_some, NULL);
		t1 = get_time_usec();
		seq_t += get_timediff(t0, t1);*/

		//assert(length(R1) == length(R2));
		for(int i = 0; i < length(R1); i++) {
			assert(VAL(R1.a[i]) > (0.0 - EPSILON));
			//assert(VAL(R2.a[i]) > (0.0 - EPSILON));
		}

		free(R1.a);
		free(P_absed.a);
		/*free(R2.a);
		free(S_absed.a);*/
	}
	//seq_t /= N_TESTS;
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
		if(IS_SOME(in.a[i]) && (VAL(in.a[i]) < (0.0 - EPSILON)) ) {
			R.a[i] = SOME( -VAL(in.a[i]) );
		} else {
			R.a[i] = in.a[i];
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

	init_par_env();

	log_benchmark(seq_abs(A), par_abs(A), T1, "abs");

	destroy_par_env();

	return 0;
}
