#include "../parallel.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <math.h>

void select_thrd(distribution dist, int id, par_array* out, void* f, void* p, void* args, void* cmp) {
	int size = dist.b_size[id];

	int (*pred)(int i, const par_array x, void* cmp) =
		(int (*)(int, const par_array, void* cmp)) p;

	const par_array* A = dist.arrs;
	maybe* A_values = dist.arrs[0].a;
	int arrbase = dist.m + dist.blocks[id];


	for(int i = arrbase; i < arrbase + size; i++) {
		if(SATISFIES(pred, i, *A, cmp))
			out->a[G2L(*out, i)] = A_values[G2L(*A, i)];
	}
}

par_array par_select(const par_array a, int m, int n, int (*p)(int i, const par_array x, void* cmp), void* cmp) {
	distribution dist;
	par_array res_array = mk_array(NULL, m, n);

	dist = distribute(&a, 1, a.m, a.n);
	set_dist_size(&dist, m, n);

	execute_in_parallel(select_thrd, dist, &res_array, NULL, p, NULL, cmp);
	free_distribution(dist);


	return res_array;
}
