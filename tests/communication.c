
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#include <runtime.h>
#include "benchmark.h"

#define EPSILON 0.0001
#define T1 800000
#define T2 62441 
#define T3 63595 
#define T4 800000
#define N_TESTS 100
#define MAX 10

int lt5(int i, const par_array x, void* cmp) {
	return IS_SOME(ELEM(x, i)) && (VAL(ELEM(x, i)) < 5.0);
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


	for(int i = 0; i < T1; i++) {
		arr1[i] = (double)rand()/(double)(RAND_MAX / MAX);
	}

	const par_array A = mk_array(arr1, 0, T1-1);

	vb_init_par_env();
	printf("#### TEST 1 ####\n");
	seq_t = 0.0;
	par_t = 0.0;
	for(int i = 0; i < N_TESTS; i++) {
		t0 = get_time_usec();
		R1 = vb_get(A, src1, lt5, NULL);
		t1 = get_time_usec();
		par_t += get_timediff(t0, t1);

		t0 = get_time_usec();
		R2 = mk_array(NULL, A.m, A.n);
		for(int j = R2.m; j < R2.m + length(R2); j++) {
			int src_i = src1(j);
			if(lt5(src_i, A, NULL)) {
				R2.a[G2L(R2, j)] = A.a[G2L(A, src_i)];	
			} else {
				R2.a[G2L(R2, j)] = NONE;
			}
		}
		t1 = get_time_usec();
		seq_t += get_timediff(t0, t1);

		for(int i = 0; i < T1; i++) {
			// Source function takes a global index, so we need to convert i (which is
			// a local index) to global space, and then apply the src function on that,
			// and then convert the resulting index back to local space.........
			cnv = G2L(A, src1(L2G(A, i)) );
			if(lt5(src1(L2G(A, i)), A, NULL)) {
				assert(abs(VAL(R1.a[i]) - VAL(A.a[cnv])) <= EPSILON);
				assert(abs(VAL(R2.a[i]) - VAL(A.a[cnv])) <= EPSILON);
			}
		}

		free(R1.a);
		free(R2.a);
	}
	vb_destroy_par_env();

	seq_t /= N_TESTS;
	par_t /= N_TESTS;

	log_benchmark(seq_t, par_t, T1, "get");
		
	return 0;
}
