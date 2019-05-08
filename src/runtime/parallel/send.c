#include "../parallel.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void send_thrd(distribution dist, int id, par_array* out, void* f, void* p, void* args) {
	int size = dist.b_size[id];

	int (*func)(int i) =
		(int (*)(int i)) f;
	int (*pred)(int i, double x) =
		(int (*)(int, double)) p;

	par_array* dest_array = (par_array*)args;

	const par_array* A = dist.arrs;
	maybe* A_values = A->a;
	int base = A->m + dist.blocks[id];

	int dst_i = 0;

	for(int i = base; i < base + size; i++) {
		// Check predicate here
		if(SATISFIES(pred, i, VAL(A_values[ G2L(*A, i) ]))) {
			dst_i = func(i);
			dest_array->a[G2L(*dest_array, dst_i)] = A_values[ G2L(*A, i) ];	
		}
	}
}

void par_send(par_array a, int (*f)(int i), const par_array b, int (*p)(int i, double lhs, double rhs)) {
	distribution dist;

	dist = distribute(&b, 1, DISTRIBUTION_STRICT);

	execute_in_parallel(send_thrd, dist, NULL, f, p, &a);
	free_distribution(dist);
}
