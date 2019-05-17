#include "../parallel.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void send_thrd(distribution dist, int id, par_array* out, void* f, void* p, void* args, void* cmp) {
	int size = dist.b_size[id];

	int (*func)(int i) =
		(int (*)(int i)) f;
	int (*pred)(int i, const par_array lhs, const par_array rhs, void* cmp) =
		(int (*)(int, const par_array, const par_array, void*)) p;

	par_array* dest_array = (par_array*)args;

	const par_array* A = dist.arrs;
	maybe* A_values = A->a;
	int base = A->m + dist.blocks[id];

	int dst_i = 0;

	for(int i = base; i < base + size; i++) {
		// Check predicate here
		if(IS_SOME(A_values[ G2L(*A, i) ]) && SATISFIES(pred, i, *dest_array, *A, cmp)) {
			dst_i = func(i);
			dest_array->a[G2L(*dest_array, dst_i)] = A_values[ G2L(*A, i) ];	
		}
	}
}

void vb_send(par_array a, int (*f)(int i), const par_array b, int (*p)(int i, const par_array lhs, const par_array rhs, void* cmp), void* cmp) {
	distribution dist;

	dist = distribute(&b, 1, b.m, b.n);

	execute_in_parallel(send_thrd, dist, NULL, f, p, &a, cmp);
	free_distribution(dist);
}
