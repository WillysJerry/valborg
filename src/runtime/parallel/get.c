#include "../parallel.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void get_thrd(distribution dist, int id, par_array* out, void* f, void* p, void* args) {
	int size = dist.b_size[id];

	int (*func)(int i) =
		(int (*)(int i)) f;
	int (*pred)(int i, double x) =
		(int (*)(int, double)) p;

	const par_array* A = dist.arrs;
	maybe* A_values = A->a;
	int base = dist.blocks[id];

	int src_i = 0;

	for(int i = base; i < base + size; i++) {
		src_i = func( L2G(*A, i) );

		// Check predicate here
		if(SATISFIES(pred, src_i, VAL(A_values[ G2L(*A, src_i) ])))
			out->a[i] = A_values[ G2L(*A, src_i) ];	
		else
			out->a[i] = NONE;
	}
}

par_array par_get(const par_array a, int (*f)(int i), int (*p)(int i, double x)) {
	distribution dist;
	par_array res_array = mk_array(NULL, a.m, a.n);

	dist = distribute(&a, 1, DISTRIBUTION_STRICT);

	execute_in_parallel(get_thrd, dist, &res_array, f, p, NULL);
	free_distribution(dist);

	return res_array;
}
