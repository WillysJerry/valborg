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

void seq_send(par_array a, int (*f)(int i), const par_array b) {
	int x = f(5);
	printf("seq_send: function returned %d\n", x);
}
