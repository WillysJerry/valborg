#include "../parallel.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void asn_thrd(distribution dist, int id, par_array* out, void* f, void* p, void* args, void* cmp) {
	int size = dist.b_size[id];

	const par_array* A = dist.arrs;
	const par_array* B = dist.arrs + 1;
	maybe* A_values = A->a;
	maybe* B_values = B->a;
	int base = dist.m + dist.blocks[id];

	int (*pred)(int i, const par_array lhs, const par_array rhs, void* cmp) =
		(int (*)(int, const par_array, const par_array, void*)) p;

	for(int i = base; i < base + size; i++) {
		if(SATISFIES(pred, i, *A, *B, cmp)) {
			out->a[G2L(*out, i)] = B_values[G2L(*B, i)];
		} else {
			out->a[G2L(*out, i)] = A_values[G2L(*A, i)];
		}
	}
}

par_array vb_asn(const par_array a, const par_array b, int (*p)(int i, par_array lhs, par_array rhs, void* cmp), void* cmp) {

	par_array arrs[] = {a, b};
	if(a.m != b.m || a.n != b.n) {
		fprintf(stderr, "[ASN] Input array bounds differ.\n");
		exit(1);
	}
	distribution dist = distribute(arrs, 2, a.m, a.n);
	par_array res = mk_array(NULL, dist.m, dist.n);

	execute_in_parallel(asn_thrd, dist, &res, NULL, p, NULL, cmp);
	free_distribution(dist);

	return res;
}
