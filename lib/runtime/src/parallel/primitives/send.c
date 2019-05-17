#include "../parallel.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct _send_args {
	par_array* dest_array;
	void* func_args;
} send_args;

void send_thrd(distribution dist, int id, par_array* out, void* f, void* p, void* args, void* cmp) {
	int size = dist.b_size[id];

	int (*func)(int i, void* arg) =
		(int (*)(int i, void* arg)) f;
	int (*pred)(int i, const par_array lhs, const par_array rhs, void* cmp) =
		(int (*)(int, const par_array, const par_array, void*)) p;

	send_args send_a = *(send_args*)args;
	par_array* dest_array = send_a.dest_array;
	void* func_args = send_a.func_args;

	const par_array* A = dist.arrs;
	maybe* A_values = A->a;
	int base = A->m + dist.blocks[id];

	int dst_i = 0;

	for(int i = base; i < base + size; i++) {
		// Check predicate here
		if(IS_SOME(A_values[ G2L(*A, i) ]) && SATISFIES(pred, i, *dest_array, *A, cmp)) {
			dst_i = func(i, func_args);
			dest_array->a[G2L(*dest_array, dst_i)] = A_values[ G2L(*A, i) ];	
		}
	}
}

void vb_send(par_array a, int (*f)(int i, void* arg), void* arg, const par_array b, int (*p)(int i, const par_array lhs, const par_array rhs, void* cmp), void* cmp) {
	distribution dist;

	dist = distribute(&b, 1, b.m, b.n);
	send_args send_a = { &a, arg };

	execute_in_parallel(send_thrd, dist, NULL, f, p, &send_a, cmp);
	free_distribution(dist);
}
