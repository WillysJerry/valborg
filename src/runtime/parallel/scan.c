#include "../parallel.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <math.h>

// Masks out elements based on input predicate from the array before performing the scan
void scan_msk_thrd(distribution dist, int id, par_array* out, void* f, void* p, void* args) { 
	int size = dist.b_size[id];

	// Just assume that these arrays were passed...
	par_array* A = dist.arrs;
	maybe* A_values = A->a;

	// Cast void pointers to functions, this is not very readable
	int (*pred)(int i, double x) =
		(int (*)(int, double)) p;

	int base = A->m + dist.blocks[id];

	for(int i = base; i < base + size; i++) {
		if(SATISFIES(pred, i, VAL(A_values[ G2L(*A, i) ]))) {
			out->a[ G2L(*out, i) ] = A_values[ G2L(*A, i) ];	
		} else {
			out->a[ G2L(*out, i) ] = NONE;
		}
	}
}

void scan_thrd(distribution dist, int id, par_array* out, void* f, void* p, void* args) {
	int size = dist.b_size[id];

	par_array* A = dist.arrs;
	maybe* A_values = A->a;

	double (*func)(double x, double y) =
		(double (*)(double x, double y)) f;

	int lvl = (int)args;

	int base = dist.m + dist.blocks[id];

	maybe x, y;
	maybe res;
	int k = 0x1 << lvl;		// 2^j 

	for(int i = base; i < base + size; i++) {
		if(i >= A->m + k) {
			x = A_values[ G2L(*A, i - k) ];
			y = A_values[ G2L(*A, i) ];

			if(IS_SOME(x) && IS_SOME(y)) {
				res = SOME( func( VAL(x), VAL(y) ) );
			} else if(IS_NONE(x) && IS_SOME(y)) {
				res = y;
			} else if(IS_SOME(x) && IS_NONE(y)) {
				res = x;
			} else {
				// Both x and y are NONE
				res = NONE;
			}

			out->a[ G2L(*out, i) ] = res;
		}
		else
			out->a[ G2L(*out, i) ] = A_values[ G2L(*A, i) ];
	}
}


par_array par_scan(double (*f)(double x, double y), const par_array a, int (*p)(int i, double x)) {
	distribution dist;
	par_array work_arrays[2];
	int in = 0, out = 0;


	dist = distribute(&a, 1, DISTRIBUTION_STRICT);
	if(p != NULL) {
		//work_arrays[0] = execute_in_parallel(scan_msk_thrd, dist, a.m, a.n, NULL, (void*)p, NULL);
		execute_in_parallel(scan_msk_thrd, dist, work_arrays, NULL, (void*)p, NULL);
		//free_distribution(dist);
	} else {
		work_arrays[0] = clone_array(a, a.m, a.n);
		work_arrays[1] = clone_array(a, a.m, a.n);
	}

	//work_arrays[1] = mk_array(NULL, a.m, a.n);
	//

	set_dist_size(&dist, a.m, a.n);
	for(int i = 0; i < ceil(log2(length(a))); i++) {
		printf("%d: %d\n", i, dist.n - dist.m + 1);
		in = i % 2;
		out = (i + 1) % 2;
		//dist = distribute(work_arrays + in, 1, DISTRIBUTION_STRICT);
		dist.arrs = work_arrays + in;

		// TODO: Need to shrink the area that gets operated on in parallel at every
		// iteration so that we don't need to go through the entire array every time.
		execute_in_parallel(scan_thrd, dist, work_arrays + out, (void*)f, NULL, (void*)(i));
		set_dist_size(&dist, a.m + (0x1 << (i)), a.n);

		//free(work_arrays[in].a);
	}
	free_distribution(dist);

	free(work_arrays[in].a);
	return work_arrays[out];
}
