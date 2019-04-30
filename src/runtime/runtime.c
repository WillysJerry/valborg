#include "runtime.h"

#include <stdlib.h>
#include <string.h> // for memcpy

// Create a parallel array of floats
par_array mk_array(const double* a, int m, int n) {
	int len = n - m + 1;
	double* v = (double*)calloc(len, sizeof(double));
	memcpy(v, a, len * sizeof(double));

	par_array arr = { m, n, v};
	return arr;
}

// Returns the length of a
int length(const par_array a) {
	return a.n - a.m + 1;
}
