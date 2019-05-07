#include "runtime.h"

#include <stdlib.h>
#include <string.h> // for memcpy

// Create a parallel array of floats
par_array mk_array(const double* a, int m, int n) {
	int len = n - m + 1;
	double* v = (double*)calloc(len, sizeof(double));

	if(a != NULL)
		memcpy(v, a, len * sizeof(double));

	par_array arr = { m, n, v};
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

void par_array_set(const par_array a, int gi, double v) {
	a.a[global_to_local(a, gi)] = v;
}
double par_array_get(const par_array a, int gi) {
	return a.a[global_to_local(a, gi)];
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
