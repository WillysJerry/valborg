#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>

#include "../src/runtime/sequential.h"
#include "../src/runtime/parallel.h"
#include "../src/runtime/runtime.h"

#define LRG_NUM 800000
#define MAX 1000

double sum2(double x, double y) {
	return x+y;
}

double mul2(double x, double y) {
	return x*y;
}

double get_time_usec() {
	static struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000.0 + tv.tv_usec;
}

int main(int argc, char** argv) {
	double r, r2; 	// Resulting scalar
	double t0, t1;
	
	double arr1[] = { 	1.0, 	2.0, 	3.0, 	4.0,	5.0,	6.0,	7.0,	8.0 };
	double arr2[LRG_NUM];

	const par_array A = mk_array(arr1, 0, 7);

	srand(0xbabeface);
	for(int i = 0; i < LRG_NUM; i++) {
		arr2[i] = (double)rand()/(double)(RAND_MAX / MAX);
	}
	const par_array B = mk_array(arr2, 0, LRG_NUM-1);

	printf("#### TEST 1 ####\n");
	r = par_reduce(mul2, A);
	r2 = seq_reduce(mul2, A);
	printf("%f %f\n", r, r2);

	printf("#### TEST 2 ####\n");
	t0 = get_time_usec();
	r = par_reduce(sum2, B);
	t1 = get_time_usec();
	printf("Parallel execution time: %.1lfus\n", t1-t0);

	t0 = get_time_usec();
	r2 = seq_reduce(sum2, B);
	t1 = get_time_usec();
	printf("Sequential execution time: %.1lfus\n", t1-t0);
	printf("%f %f\n", r, r2);
	return 0;
}
