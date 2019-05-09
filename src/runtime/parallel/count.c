#include "../parallel.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <math.h>

void count_thrd(distribution dist, int id, par_array* out, void* f, void* p, void* args) {
	int size = dist.b_size[id];

	int (*pred)(int i, par_array x) =
		(int (*)(int, par_array)) p;

	par_array* A = dist.arrs;
	maybe* A_values = A->a + dist.blocks[id];
	int arrbase = dist.m + dist.blocks[id];


	out->a[id] = SOME(0);
	for(int i = 0; i < size; i++) {
		if(IS_SOME(A_values[i]) && SATISFIES(pred, arrbase + i, *A))
			out->a[id] = SOME( VAL(out->a[id]) + 1);
	}
}

int par_count(const par_array a, int (*p)(int i, par_array x)) {
	distribution dist;
	par_array res_array = mk_array(NULL, 0, NUM_THREADS-1);
	int result;

	dist = distribute(&a, 1, DISTRIBUTION_STRICT);

	execute_in_parallel(count_thrd, dist, &res_array, NULL, p, NULL);
	free_distribution(dist);

	// Need to round to integer to account for float (double) imprecision
	result = round(VAL(res_array.a[0]));
	for(int i = 1; i < NUM_THREADS; i++) {
		result += round(VAL(res_array.a[i]));
	}

	free(res_array.a);

	return result;
}
