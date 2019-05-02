#include "../parallel.h"
#include "threading.h"

#include <stdlib.h>
#include <string.h>

#include <stdio.h>
void map3_thrd(distribution dist, int id, dist_ret* retval, void* f, void* p) {
	int size = dist.b_size[id];
	double* arr = (double*)calloc(dist.b_size[id], sizeof(double));

	// Just assume that these arrays were passed...
	double* A = dist.arrs[0];
	double* B = dist.arrs[1];
	double* C = dist.arrs[2];

	// Cast void pointers to functions, this is not very readable
	double (*func)(double x, double y, double z) =
		(double (*)(double x, double y, double z)) f;
	int (*pred)(int i) =
		(int (*)(int)) p;


	for(int i = 0; i < size; i++) {
		if(pred == NULL || pred(i)) {
			arr[i] = func(A[i], B[i], C[i]);
			printf("[%d][%d] %f*%f*%f=%f ", id, i, A[i], B[i], C[i], arr[i]);
		}
	}	

	retval->v = arr;
	retval->n = size;
}

par_array par_map3(double (*f)(double x, double y, double z), const par_array a, const par_array b, const par_array c, int (*p)(int i)) {
	par_array result;
	par_array arrs[] = { a, b, c };
	distribution dist = distribute(arrs, 3);

	print_distribution(dist);


	// Start parallel execution
	dist_ret** ret = execute_in_parallel(map3_thrd, dist, (void*)f, (void*)p);

	result.a = NULL;	// If none of the input arrays index sets intersect, we'll return a NULL pointer 
	result.m = dist.m;	
	result.n = dist.m - 1;	// Initialize n to be one less then m, as n,m are both inclusive (m=0,n=0 equals an array consisting of one element

	int cnt = 0;
	if(ret != NULL) {
		// Get the total length of the output array
		for(int i = 0; i < NUM_THREADS; i++) {
			if(ret[i] != NULL)
				result.n += ret[i]->n;
		}
		result.a = (double*)calloc(length(result), sizeof(double));
		for(int i = 0; i < NUM_THREADS; i++) {
			if(ret[i] != NULL) {
				for(int j = 0; j < ret[i]->n; j++)
					result.a[cnt++] = ret[i]->v[j];
				free(ret[i]->v);
				free(ret[i]);
			}
		}
		free(ret);
	}

	printf("##### MAP3 RESULT #####\n");
	for(int i = 0; i < length(result); i++) {
		printf("%f ", result.a[i]);
	}
	printf("\n");
	return result;
}
