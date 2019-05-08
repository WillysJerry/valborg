#include "../parallel.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
 * Individual worker functions (Local-view)
 */
void map_thrd(distribution dist, int id, par_array* out, void* f, void* p, void* args) {
	int size = dist.b_size[id];

	// Just assume that these arrays were passed...
	par_array* A = dist.arrs;
	maybe* A_values = dist.arrs[0].a; // + dist.blocks[id];

	// Cast void pointers to functions, this is not very readable
	double (*func)(double x) =
		(double (*)(double x)) f;
	int (*pred)(int i, double x) =
		(int (*)(int, double)) p;

	maybe x;
	int glob;

	for(int i = dist.blocks[id]; i < dist.blocks[id] + size; i++) {
		glob = L2G(*out, i);
		x = A_values[G2L(*A, glob)]; 
		if(IS_SOME(x) && SATISFIES(pred, i, VAL(x))) {
			out->a[i] = SOME( func( VAL(x) ) );
		}
		else {
			out->a[i] = NONE;
		}
	}	
}

void map2_thrd(distribution dist, int id, par_array* out, void* f, void* p, void* args) {
	int size = dist.b_size[id];

	// Just assume that these arrays were passed...
	par_array* A = dist.arrs;
	par_array* B = dist.arrs + 1;

	maybe* A_values = A->a;
	maybe* B_values = B->a;

	// Cast void pointers to functions, this is not very readable
	double (*func)(double x, double y) =
		(double (*)(double, double)) f;
	int (*pred)(int i, double x, double y) =
		(int (*)(int, double, double)) p;

	maybe x;
	maybe y;
	int glob;


	for(int i = dist.blocks[id]; i < dist.blocks[id] + size; i++) {
		glob = L2G(*out, i);
		x = A_values[G2L(*A, glob)]; 
		y = B_values[G2L(*B, glob)];

		if(IS_SOME(x)  && 
		    IS_SOME(y) && 
		    SATISFIES(pred, i, VAL(x), VAL(y))) {

			out->a[i] = SOME( func( VAL(x), VAL(y) ) );
		}
		else {
			out->a[i] = NONE;
		}
	}	
}

void map3_thrd(distribution dist, int id, par_array* out, void* f, void* p, void* args) {
	int size = dist.b_size[id];

	// Just assume that these arrays were passed...
	par_array* A = dist.arrs;
	par_array* B = dist.arrs + 1;
	par_array* C = dist.arrs + 2;

	maybe* A_values = A->a;
	maybe* B_values = B->a;
	maybe* C_values = C->a;

	// Cast void pointers to functions, this is not very readable
	double (*func)(double x, double y, double z) =
		(double (*)(double, double, double)) f;
	int (*pred)(int i, double x, double y, double z) =
		(int (*)(int, double, double, double)) p;

	maybe x, y, z;
	int glob;


	for(int i = dist.blocks[id]; i < dist.blocks[id] + size; i++) {
		glob = L2G(*out, i);
		x = A_values[G2L(*A, glob)]; 
		y = B_values[G2L(*B, glob)];
		z = C_values[G2L(*C, glob)];

		if(IS_SOME(x)  && 
		    IS_SOME(y) && 
		    IS_SOME(z) &&
		    SATISFIES(pred, i, VAL(x), VAL(y), VAL(z))) {

			out->a[i] = SOME( func( VAL(x), VAL(y), VAL(z) ) );
		}
		else {
			out->a[i] = NONE;
		}
	}	
}

/*
 * Global-view map functions
 */
par_array par_map1(double (*f)(double x), const par_array a, int (*p)(int i, double x)) {
	par_array result;

	par_array arrs[] = { a };
	distribution dist = distribute(arrs, 1, DISTRIBUTION_INTERSECTION);


	// Start parallel execution
	par_array ret = execute_in_parallel(map_thrd, dist, dist.m, dist.n, (void*)f, (void*)p, NULL);

	return ret;

}

par_array par_map2(double (*f)(double x, double y), const par_array a, const par_array b, int (*p)(int i, double x, double y)) {
	par_array result;

	par_array arrs[] = { a, b };
	distribution dist = distribute(arrs, 2, DISTRIBUTION_INTERSECTION);


	// Start parallel execution
	par_array ret = execute_in_parallel(map2_thrd, dist, dist.m, dist.n, (void*)f, (void*)p, NULL);

	return ret;

}

par_array par_map3(double (*f)(double x, double y, double z), const par_array a, const par_array b, const par_array c, int (*p)(int i, double x, double y, double z)) {
	par_array result;

	par_array arrs[] = { a, b, c };
	distribution dist = distribute(arrs, 3, DISTRIBUTION_INTERSECTION);


	// Start parallel execution
	par_array ret = execute_in_parallel(map3_thrd, dist, dist.m, dist.n, (void*)f, (void*)p, NULL);

	return ret;

}
