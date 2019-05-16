#include "../parallel.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <math.h>

void count_thrd(distribution dist, int id, par_array* out, void* f, void* p, void* args, void* cmp) {
	int size = dist.b_size[id];

	int (*pred)(int i, const par_array x, void* cmp) =
		(int (*)(int, const par_array, void*)) p;

	par_array* A = dist.arrs;
	maybe* A_values = A->a;
	int arrbase = dist.m + dist.blocks[id];

	int cnt = 0;


//	out->a[id] = SOME(0.0);
	for(int i = arrbase; i < arrbase + size; i++) {
		if(IS_SOME(ELEM(*A, i)) && SATISFIES(pred, i, *A, cmp))
			cnt += 1;
	}
	//out->a[id] = SOME( VAL(out->a[id]) + 1.0);
	out->a[id] = SOME((double)cnt);
}

int vb_count(const par_array a, int (*p)(int i, par_array x, void* cmp), void* cmp) {
	distribution dist;
	par_array res_array = mk_array(NULL, 0, NUM_THREADS-1);
	int result;

	dist = distribute(&a, 1, a.m, a.n);

	execute_in_parallel(count_thrd, dist, &res_array, NULL, p, NULL, cmp);
	free_distribution(dist);

	// Need to round to integer to account for float (double) imprecision
	result = round(VAL(res_array.a[0]));
	for(int i = 1; i < NUM_THREADS; i++) {
		result += round(VAL(res_array.a[i]));
	}

	free(res_array.a);

	return result;
}
