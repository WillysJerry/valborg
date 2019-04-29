#include "runtime.h"

#include <stdlib.h>
#include <string.h> // for memcpy

par_array mk_char_array(const char* a, int m, int n) {
	int len = n - m + 1;
	p_u v;
	v._c = (char*)calloc(len, sizeof(char)); 
	memcpy(v._c, a, len * sizeof(char));

	par_array arr = { m, n, v};
	return arr;
}	

par_array mk_int_array(const int* a, int m, int n) {
	int len = n - m + 1;
	p_u v;
	v._i = (int*)calloc(len, sizeof(int)); 
	memcpy(v._i, a, len * sizeof(int));

	par_array arr = { m, n, v};
	return arr;
}	

// Returns the length of a
int length(const par_array a) {
	return a.n - a.m + 1;
}
