
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
#define T1 804200
#define T2 62441 
#define T3 63595 
#define T4 800000
#define N_TESTS 1000
#define MAX 10

int lt5(int i, double x) {
	return x < 5.0;
}

int dst(int i) {
	return i;
}

int src1(int i) {
	return (i * 3 - 1) % T1;
}

int main(int argc, char** argv) {
	double t0, t1;
	double seq_t, par_t;
	int cnv;

	par_array R1, R2; 	// Resulting array

	double arr1[T1];

	init_par_env();

	for(int i = 0; i < T1; i++) {
		arr1[i] = (double)rand()/(double)(RAND_MAX / MAX);
	}

	const par_array A = mk_array(arr1, 0, T1-1);

	printf("#### TEST 1 ####\n");
	seq_t = 0.0;
	par_t = 0.0;
	for(int i = 0; i < N_TESTS; i++) {
		t0 = get_time_usec();
		R1 = par_get(A, src1, lt5);
		t1 = get_time_usec();
		par_t += get_timediff(t0, t1);

		t0 = get_time_usec();
		R2 = seq_get(A, src1, lt5);
		t1 = get_time_usec();
		seq_t += get_timediff(t0, t1);

		for(int i = 0; i < T1; i++) {
			// Source function takes a global index, so we need to convert i (which is
			// a local index) to global space, and then apply the src function on that,
			// and then convert the resulting index back to local space.........
			cnv = G2L(A, src1(L2G(A, i)) );
			if(lt5(i, VAL(A.a[cnv]))) {
				assert(abs(VAL(R1.a[i]) - VAL(A.a[cnv])) <= EPSILON);
				assert(abs(VAL(R2.a[i]) - VAL(A.a[cnv])) <= EPSILON);
			}
		}

		free(R1.a);
		free(R2.a);
	}
	seq_t /= N_TESTS;
	par_t /= N_TESTS;

	log_benchmark(seq_t, par_t, T1, "get");
		
	destroy_par_env();
	return 0;
}
