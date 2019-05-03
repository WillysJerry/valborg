#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "../src/runtime/sequential.h"
#include "../src/runtime/parallel.h"
#include "../src/runtime/runtime.h"

#define SIZE 10000

int even(int i) {
	return i % 2 == 0;
}

int dst(int i) {
	return i;
}

int src(int i) {
	return (i * 3 - 1) % SIZE;
}

int main(int argc, char** argv) {
	par_array R; 	// Resulting array

	double arr1[SIZE];
	double arr2[SIZE];

	for(int i = 0; i < SIZE; i++) {
		arr1[i] = i;
		arr2[i] = 0.0;
	}

	const par_array A = mk_array(arr1, 0, SIZE-1);

	par_array B = seq_get(A, dst, NULL);
	for(int i = 0; i < SIZE; i++) {
		assert(B.a[i] == A.a[i]);
	}

	par_array C = mk_array(arr2, 0, SIZE-1);

	seq_send(C, src, A, even);

	for(int i = 0; i < SIZE; i++) {
		if(i % 2 == 0)
			assert(C.a[i] == A.a[src(i)]);
		else
			assert(C.a[i] == 0.0);
	}
		

	return 0;
}
