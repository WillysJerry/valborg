#include "../parallel.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <math.h>

// First scan sweep: each processor performs a scan on its local elements, and masks out those elements that do not fulfill predicate p
void scan1_thrd(distribution dist, int id, par_array* out, void* f, void* p, void* args, void* cmp) { 
	int size = dist.b_size[id];

	const par_array* A = dist.arrs;

	double (*func)(double x, double y) =
		(double (*)(double x, double y)) f;
	int (*pred)(int i, const par_array x, void* cmp) =
		(int (*)(int, const par_array, void*)) p;

	int base = dist.m + dist.blocks[id];

	double acc = 0.0;
	int i;
	for(i = base; i < base + size; i++) {
		if(IS_SOME(ELEM(*A, i)) && SATISFIES(pred, i, *A, cmp)) {
			acc = VAL(ELEM(*A, i));
			out->a[G2L(*out, i)] = SOME(acc);
			i++;
			break;
		}
		else {
			out->a[G2L(*out, i)] = NONE;
		}
	}
	for(; i < base + size; i++) {
		if(IS_SOME(ELEM(*A, i)) && SATISFIES(pred, i, *A, cmp)) {
			acc = func(acc, VAL(ELEM(*A, i)));
		}
		out->a[G2L(*out, i)] = SOME(acc);

	}
}

// Second scan sweep: each processor takes into account the results of the first sweep from those blocks with a lower id
void scan2_thrd(distribution dist, int id, par_array* out, void* f, void* p, void* args, void* cmp) { 
	int size = dist.b_size[id];

	const par_array* A = dist.arrs;

	double (*func)(double x, double y) =
		(double (*)(double x, double y)) f;

	int base = dist.m + dist.blocks[id];

	int i;
	maybe prev_sum;
	maybe pi_sum;
	// Combine the resulting values of the scan operations performed on the previous blocks
	for(i = 0; i < id; i++) {
		pi_sum = ELEM(*A, dist.m + dist.blocks[i] + dist.b_size[i] - 1);
		if(IS_SOME(pi_sum)) {
			prev_sum = pi_sum;
			i++;
			break;
		}
	}
	for(; i < id; i++) {
		pi_sum = ELEM(*A, dist.m + dist.blocks[i] + dist.b_size[i] - 1);
		if(IS_SOME(pi_sum)) {
			prev_sum = SOME(func(VAL(prev_sum), VAL(pi_sum)));
		}
	}

	// Apply function f on the combined results of the previous blocks and each element in this block (if any of the earlier blocks resulted in a SOME)
	if(IS_SOME(prev_sum)) {
		for(i = base; i < base + size; i++) {
			out->a[G2L(*out, i)] = SOME( func(VAL(prev_sum), VAL(ELEM(*A, i))));
		}
	}
}

par_array vb_scan(double (*f)(double x, double y), const par_array a, int (*p)(int i, par_array x, void* cmp), void* cmp) {
	distribution dist;
	par_array work_arrays[2];

	dist = distribute(&a, 1, a.m, a.n);
	work_arrays[0] = mk_array(NULL, a.m, a.n);
	work_arrays[1] = mk_array(NULL, a.m, a.n);

	execute_in_parallel(scan1_thrd, dist, work_arrays, (void*)f, (void*)p, NULL, cmp);

	free_distribution(dist);

	// TODO: The second sweep should be able to ignore the results of the first block (the elements at these indices will stay the same after the second sweep regardless), at the moment the first processor only copies the values from the previous sweep to the output. This probably requires a rewrite of the second sweep though as it uses the block indices from the previous distribution.
	dist = distribute(work_arrays, 1, a.m, a.n);
	execute_in_parallel(scan2_thrd, dist, work_arrays + 1, (void*)f, NULL, NULL, NULL);
	free_distribution(dist);

	free(work_arrays[0].a);
	return work_arrays[1];
}
