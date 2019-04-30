#include "../sequential.h"
#include "../runtime.h"

#include <stdlib.h>
#include <string.h>

// 'p' is a pointer to a predicate used to mask out certain elements. It works kind of weird right now as it (on purpose) "squishes" the resulting array to ONLY contain the non-masked elements of 'a', which makes the mapping function act kind of funny (maybe). The predicate can also be NULL which makes this function perform a normal get communication.
par_array seq_get(const par_array a, int (*f)(int i), int (*p)(double x)) {

	int len = length(a);
	int n = 0;		// Size counter of the resulting array.

	// Allocate temp array
	double* arr = (double*)calloc(len, sizeof(double));
	
	for(int i = 0; i < len; i++) {
		// Make sure a[f(i)] satisfies predicate
		if(p == NULL || p(a.a[f(i)]))
			memcpy(	arr + (n++),  		// dest
				a.a + f(i), 		// src
				sizeof(double));	// size
	}

	par_array b = mk_array(arr, a.m, a.m + n - 1); 
	//par_array b = mk_array(arr, a.m, a.n); 
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

par_array seq_map1(double (*f)(double x), const par_array a) {
	int len = length(a);
	double* arr = (double*)calloc(len, sizeof(double));

	for(int i = 0; i < len; i++) {
		arr[i] = f(a.a[i]);
	}

	par_array res = mk_array(arr, a.m, a.n);
	free(arr);

	return res;
}

// How to handle these (map2, map3)? Should arrays of different sizes be allowed? Also how will it work with index bounds?
par_array seq_map2(double (*f)(double x, double y), const par_array a, const par_array b) {
	// Make the resulting arrays length depend on the length of the first array for now. 
	int len = length(a);
	double* arr = (double*)calloc(len, sizeof(double));

	for(int i = 0; i < len; i++) {
		arr[i] = f(a.a[i], b.a[i]);
	}

	par_array res = mk_array(arr, a.m, a.n);
	free(arr);

	return res;
}

par_array seq_map3(double (*f)(double x, double y, double z), const par_array a, const par_array b, const par_array c) {
	// Make the resulting arrays length depend on the length of the first array for now. 
	int len = length(a);
	double* arr = (double*)calloc(len, sizeof(double));

	for(int i = 0; i < len; i++) {
		arr[i] = f(a.a[i], b.a[i], c.a[i]);
	}

	par_array res = mk_array(arr, a.m, a.n);
	free(arr);

	return res;
}

// Sequential reduce and scan are basically linear pairwise applications of the function f over the input array
double seq_reduce(double (*f)(double x, double y), const par_array a) {
	double res = a.a[0];
	for(int i = 1; i < length(a); i++) {
		res = f(res, a.a[i]);	
	}

	return res;
}
par_array seq_scan(double (*f)(double x, double y), const par_array a) {
	double* arr = (double*)calloc(length(a), sizeof(double));

	// a[0], f(a[0], a[1]), f(a[0], f(a[1], a[2])), ...
	arr[0] = a.a[0];
	for(int i = 1; i < length(a); i++) {
		arr[i] = f(arr[i-1], a.a[i]);	
	}

	par_array res = mk_array(arr, a.m, a.n);
	free(arr);
	return res;
}

