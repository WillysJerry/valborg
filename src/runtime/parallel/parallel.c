#include "../parallel.h"
#include "threading.h"

#include <stdlib.h>
#include <string.h>

#include <stdio.h>

void merge_result(dist_ret** ret, par_array* result) {
	int cnt = 0;

	// Get the total length of the output array
	for(int i = 0; i < NUM_THREADS; i++) {
		if(ret[i] != NULL)
			result->n += ret[i]->n;
	}
	result->a = (double*)calloc(length(*result), sizeof(double));
	for(int i = 0; i < NUM_THREADS; i++) {
		if(ret[i] != NULL) {
			for(int j = 0; j < ret[i]->n; j++)
				result->a[cnt++] = ret[i]->v[j];
			free(ret[i]->v);
			free(ret[i]);
		}
	}
}

void map_thrd(distribution dist, int id, dist_ret* retval, void* f, void* p) {
	int size = dist.b_size[id];
	double* arr = (double*)calloc(dist.b_size[id], sizeof(double));

	// Just assume that these arrays were passed...
	double* A = dist.arrs[0] + dist.blocks[id];

	// Cast void pointers to functions, this is not very readable
	double (*func)(double x) =
		(double (*)(double x)) f;
	int (*pred)(int i) =
		(int (*)(int)) p;


	for(int i = 0; i < size; i++) {
		if(pred == NULL || pred(i)) {
			arr[i] = func(A[i]);
		}
	}	

	retval->v = arr;
	retval->n = size;
}

par_array par_map1(double (*f)(double x), const par_array a, int (*p)(int i)) {
	par_array result;
	par_array arrs[] = { a };
	distribution dist = distribute(arrs, 1);

	// Start parallel execution
	dist_ret** ret = execute_in_parallel(map_thrd, dist, (void*)f, (void*)p);

	result.a = NULL;	// If none of the input arrays index sets intersect, we'll return a NULL pointer 
	result.m = dist.m;	
	result.n = dist.m - 1;	// Initialize n to be one less then m, as n,m are both inclusive (m=0,n=0 equals an array consisting of one element

	if(ret != NULL) {
		merge_result(ret, &result);
		free(ret);
	}

	return result;

}

void map2_thrd(distribution dist, int id, dist_ret* retval, void* f, void* p) {
	int size = dist.b_size[id];
	double* arr = (double*)calloc(dist.b_size[id], sizeof(double));

	// Just assume that these arrays were passed...
	double* A = dist.arrs[0] + dist.blocks[id];
	double* B = dist.arrs[1] + dist.blocks[id];

	// Cast void pointers to functions, this is not very readable
	double (*func)(double x, double y) =
		(double (*)(double x, double y)) f;
	int (*pred)(int i) =
		(int (*)(int)) p;


	for(int i = 0; i < size; i++) {
		if(pred == NULL || pred(i)) {
			arr[i] = func(A[i], B[i]);
		}
	}	

	retval->v = arr;
	retval->n = size;
}

par_array par_map2(double (*f)(double x, double y), const par_array a, const par_array b, int (*p)(int i)) {
	par_array result;
	par_array arrs[] = { a, b };
	distribution dist = distribute(arrs, 2);

	// Start parallel execution
	dist_ret** ret = execute_in_parallel(map2_thrd, dist, (void*)f, (void*)p);

	result.a = NULL;	// If none of the input arrays index sets intersect, we'll return a NULL pointer 
	result.m = dist.m;	
	result.n = dist.m - 1;	// Initialize n to be one less then m, as n,m are both inclusive (m=0,n=0 equals an array consisting of one element

	if(ret != NULL) {
		merge_result(ret, &result);
		free(ret);
	}

	return result;
}

void map3_thrd(distribution dist, int id, dist_ret* retval, void* f, void* p) {
	int size = dist.b_size[id];
	double* arr = (double*)calloc(dist.b_size[id], sizeof(double));

	// Just assume that these arrays were passed...
	double* A = dist.arrs[0] + dist.blocks[id];
	double* B = dist.arrs[1] + dist.blocks[id];
	double* C = dist.arrs[2] + dist.blocks[id];

	// Cast void pointers to functions, this is not very readable
	double (*func)(double x, double y, double z) =
		(double (*)(double x, double y, double z)) f;
	int (*pred)(int i) =
		(int (*)(int)) p;


	for(int i = 0; i < size; i++) {
		if(pred == NULL || pred(i)) {
			arr[i] = func(A[i], B[i], C[i]);
		}
	}	

	retval->v = arr;
	retval->n = size;
}

par_array par_map3(double (*f)(double x, double y, double z), const par_array a, const par_array b, const par_array c, int (*p)(int i)) {
	par_array result;
	par_array arrs[] = { a, b, c };
	distribution dist = distribute(arrs, 3);

	// Start parallel execution
	dist_ret** ret = execute_in_parallel(map3_thrd, dist, (void*)f, (void*)p);

	result.a = NULL;	// If none of the input arrays index sets intersect, we'll return a NULL pointer 
	result.m = dist.m;	
	result.n = dist.m - 1;	// Initialize n to be one less then m, as n,m are both inclusive (m=0,n=0 equals an array consisting of one element

	if(ret != NULL) {
		merge_result(ret, &result);
		free(ret);
	}

	return result;
}
