#include "../sequential.h"
#include "../runtime.h"

#include <stdlib.h>
#include <string.h>

par_array seq_get(const par_array a, int (*f)(int i)) {

	int len = length(a);

	// Allocate temp array
	double* arr = (double*)calloc(len, sizeof(double));
	
	for(int i = 0; i < len; i++) {
		memcpy(	arr + i,  		// dest
			a.a + f(i), 		// src
			sizeof(double));	// size
	}

	par_array b = mk_array(arr, a.m, a.n); 
	free(arr);

	return b;
}

// This function has side-effects. Possible race condition if f(i) == f(j) for some i and j.
// Another thing to consider is how the bounds of the two arrays should work together, maybe we only send the elements that are intersecting? For now this is ignored.
void seq_send(par_array a, int (*f)(int i), const par_array b) {
	int len = length(a);
	for(int i = 0; i < len; i++) {
		a.a[i] = b.a[f(i)];
	}
}

// How should this work with the index bounds of a and b (i.e the m and n)?
par_array seq_concat(const par_array a, const par_array b) {
	int len = length(a) + length(b);

	double* arr = (double*)calloc(len, sizeof(double));
	memcpy(arr, a.a, sizeof(double) * length(a));
	memcpy(arr + length(a), b.a, sizeof(double) * length(b));

	// Temporary solution: lower bound will start at a.m, and then add the length of the new array to get the higher bound. Maybe user should specify the new bounds somehow?
	par_array c = mk_array(arr, a.m, a.m + len - 1);
	free(arr);

	return c;
}

par_array seq_select(const par_array a, int m, int n) {
	par_array b = mk_array(a.a + m, m, n);

	return b;
}
