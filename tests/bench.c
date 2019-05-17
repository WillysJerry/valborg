#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#include <runtime.h>
#include "benchmark.h"

#define EPSILON 0.01
#define T1 100000
#define N_TESTS 100
#define MAX 10

int dst(int i) {
	return i;
}
int src(int i) {
	return i;
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

int main(int argc, char** argv) {

	double t0, t1;

	double get = 0.0, send = 0.0, concat = 0.0, select = 0.0, map1 = 0.0, map2 = 0.0, map3 = 0.0, reduce = 0.0, scan = 0.0, count = 0.0, asn = 0.0;

	double arr0[T1];
	double s;
	int cnt;

	for(unsigned long long i = 0; i < T1; i++) {
		arr0[i] = (double)rand()/(double)( (RAND_MAX / MAX) ) - (double)(MAX / 2.0);
	}

	par_array A = mk_array(arr0, 0, T1-1);
	par_array R;

	vb_init_par_env();

	for(int i = 0; i < N_TESTS; i++) {

		t0 = get_time_usec();
		R = vb_get(A, dst, NULL, NULL);
		t1 = get_time_usec();
		get += get_timediff(t0, t1);

		t0 = get_time_usec();
		vb_send(R, src, A, NULL, NULL);
		t1 = get_time_usec();
		send += get_timediff(t0, t1);
		free(R.a);

		t0 = get_time_usec();
		R = vb_concat(A, A);
		t1 = get_time_usec();
		concat += get_timediff(t0, t1);
		free(R.a);

		t0 = get_time_usec();
		R = vb_select(A, 0, T1-5, NULL, NULL);
		t1 = get_time_usec();
		select += get_timediff(t0, t1);
		free(R.a);

		t0 = get_time_usec();
		R = vb_map1(neg, A, NULL, NULL);
		t1 = get_time_usec();
		map1 += get_timediff(t0, t1);
		free(R.a);

		t0 = get_time_usec();
		R = vb_map2(sum2, A, A, NULL, NULL);
		t1 = get_time_usec();
		map2 += get_timediff(t0, t1);
		free(R.a);

		t0 = get_time_usec();
		R = vb_map3(mul3, A, A, A, NULL, NULL);
		t1 = get_time_usec();
		map3 += get_timediff(t0, t1);
		free(R.a);

		t0 = get_time_usec();
		s = vb_reduce(sum2, 0.0, A, NULL, NULL);
		t1 = get_time_usec();
		reduce += get_timediff(t0, t1);

		t0 = get_time_usec();
		R = vb_scan(sum2, 0.0, A, NULL, NULL);
		t1 = get_time_usec();
		scan += get_timediff(t0, t1);
		free(R.a);

		t0 = get_time_usec();
		cnt = vb_count(A, NULL, NULL);
		t1 = get_time_usec();
		count += get_timediff(t0, t1);

		t0 = get_time_usec();
		R = vb_asn(A, A, NULL, NULL);
		t1 = get_time_usec();
		asn += get_timediff(t0, t1);
		free(R.a);
	}

	get 	/= N_TESTS;
	send 	/= N_TESTS;
	concat 	/= N_TESTS;
	select 	/= N_TESTS;
	map1 	/= N_TESTS;
	map2 	/= N_TESTS;
	map3 	/= N_TESTS;
	reduce 	/= N_TESTS;
	scan 	/= N_TESTS;
	count 	/= N_TESTS;
	asn 	/= N_TESTS;

	printf("get: %.1f\nsend: %.1f\nconcat: %.1f\nselect: %.1f\nmap1: %.1f\nmap2: %.1f\nmap3: %.1f\nreduce: %.1f\nscan: %.1f\ncount: %.1f\nasn: %.1f\n", get, send, concat, select, map1, map2, map3, reduce, scan, count, asn);

	vb_destroy_par_env();

	return 0;
}
