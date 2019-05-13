#include "../parallel.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void replicate_thrd(distribution dist, int id, par_array* out, void* f, void* p, void* args, void* cmp) {
	int size = dist.b_size[id];
	double val = *(double*) args;

	int base = dist.m + dist.blocks[id];

	int (*pred)(int i, void* cmp) =
		(int (*)(int, void*)) p;

	for(int i = base; i < base + size; i++) {
		if(SATISFIES(pred, i, cmp)) {
			out->a[G2L(*out, i)] = SOME(val); 
		} else {
			out->a[G2L(*out, i)] = NONE;
		}
	}
}

par_array par_replicate(double v, int m, int n, int (*p)(int i, void* cmp), void* cmp) {

	par_array res = mk_array(NULL, m, n);
	distribution dist = distribute(NULL, 0, m, n);

	execute_in_parallel(replicate_thrd, dist, &res, NULL, p, (void*)&v, cmp);
	free_distribution(dist);

	return res;
}
