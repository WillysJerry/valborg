#include "../parallel.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void asn_thrd(distribution dist, int id, par_array* out, void* f, void* p, void* args) {
	int size = dist.b_size[id];

	const par_array* A = dist.arrs;
	const par_array* B = dist.arrs + 1;
	maybe* A_values = A->a;
	maybe* B_values = B->a;
	int base = dist.m + dist.blocks[id];

	int (*pred)(int i, double lhs, double rhs) =
		(int (*)(int, double, double)) p;

	for(int i = base; i < base + size; i++) {
		if(SATISFIES(pred, i, VAL(A_values[G2L(*A, i)]), VAL(B_values[G2L(*B, i)]))) {
			out->a[G2L(*out, i)] = B_values[G2L(*B, i)];
		} else {
			out->a[G2L(*out, i)] = A_values[G2L(*A, i)];
		}
	}
}

par_array par_asn(const par_array a, const par_array b, int (*p)(int i, double lhs, double rhs)) {

	par_array arrs[] = {a, b};
	distribution dist = distribute(arrs, 2, DISTRIBUTION_STRICT);
	par_array res = mk_array(NULL, a.m, a.n);

	execute_in_parallel(asn_thrd, dist, &res, NULL, p, NULL);
	free_distribution(dist);

	return res;
}
