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
#define T1 100000
#define T2 62441 
#define T3 63595 
#define T4 800000
#define N_TESTS 1000
#define MAX 10

int lt5(int i, double x, double y) {
	return y < 5.0;
}

int main(int argc, char** argv) {
	double t0, t1;
	double seq_t, par_t;

	par_array R1, R2; 	// Resulting array

	double arr1[T1];
	double arr2[T1];

	init_par_env();

	for(int i = 0; i < T1; i++) {
		arr1[i] = (double)rand()/(double)(RAND_MAX / MAX);
		arr2[i] = (double)rand()/(double)(RAND_MAX / MAX);
	}

	const par_array A1 = mk_array(arr1, 0, T1-1);
	const par_array A2 = mk_array(arr2, 0, T1-1);

	printf("#### TEST 1 ####\n");
	seq_t = 0.0;
	par_t = 0.0;
	for(int i = 0; i < N_TESTS; i++) {
		t0 = get_time_usec();
		R1 = par_asn(A1, A2, lt5);
		t1 = get_time_usec();
		par_t += get_timediff(t0, t1);

		t0 = get_time_usec();
		R2 = seq_asn(A1, A2, lt5);
		t1 = get_time_usec();
		seq_t += get_timediff(t0, t1);

		for(int i = 0; i < T1; i++) {
			if(lt5(i, VAL(A1.a[i]), VAL(A2.a[i]))) {
				assert( abs(VAL(R1.a[i]) - VAL(A2.a[i])) <= EPSILON);
			} else {
				assert( abs(VAL(R1.a[i]) - VAL(A1.a[i])) <= EPSILON);
			}
			assert( abs(VAL(R1.a[i]) - VAL(R2.a[i])) <= EPSILON);
		}

		free(R1.a);
		//free(R2.a);
	}
	seq_t /= N_TESTS;
	par_t /= N_TESTS;

	log_benchmark(seq_t, par_t, T1, "asn");
		
	destroy_par_env();
	return 0;
}
