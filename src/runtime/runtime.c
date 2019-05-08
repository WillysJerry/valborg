#include "runtime.h"

#include <stdlib.h>
#include <string.h> // for memcpy

// Create a parallel array of floats
par_array mk_array(const double* a, int m, int n) {
	int len = n - m + 1;
	//double* v = (double*)calloc(len, sizeof(double));
	maybe* v = (maybe*)calloc(len, sizeof(maybe));

	if(a != NULL) {
		for(int i = 0; i < len; i++) {
			v[i] = SOME(a[i]);
		}
		//memcpy(v, a, len * sizeof(double));
	}

	par_array arr = { m, n, v};
	return arr;
}

par_array clone_array(const par_array a, int m, int n) {
	//int len = n - m + 1;
	par_array arr = mk_array(NULL, m, n);

	for(int i = m; i < n + 1; i++) {
		arr.a[G2L(arr, i)] = a.a[G2L(a, i)];
	}

	return arr;

}

// Returns the length of a
int length(const par_array a) {
	return a.n - a.m + 1;
}

// Indices in the range [m..n] to [0..length(a))
int global_to_local(const par_array a, int i) {
	return i - a.m;
}
// Indices in the range [0..length(a)) to [m..n]
int local_to_global(const par_array a, int i) {
	return a.m + i;
}

bounds intersection(const par_array* arrs, int n) {
	bounds b;
	
	// Figure out where the arrays intersect
	b.m = arrs[0].m;
	b.n = arrs[0].n;

	for(int i = 1; i < n; i++) {
		if(arrs[i].m > b.m)
			b.m = arrs[i].m;	
		if(arrs[i].n < b.n)
			b.n = arrs[i].n;
	}

	return b;
}
