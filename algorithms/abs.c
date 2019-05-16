// Absolute value of an array
//
// Input: 
// 	A = [1.0, -2.0, 3.0, 4.0, -5.0]
// Do: 	
// 	forall k where A[k] < 0.0 do A[k] = -A[k] 
// Expected output: 
// 	[1.0, 2.0, 3.0, 4.0, 5.0]

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#include "../src/runtime/parallel/parallel.h"
#include "../src/runtime/runtime.h"

int same(int i) {
	return i;
}

int main(int argc, char** argv) {

	double arr0[] = { 1.0, -2.0, 3.0, 4.0, -5.0 };

	par_array A = mk_array(arr0, 0, 4);

	init_par_env();
		// Apply fabs (abs for floating point values) elementwise on every element in A
		par_array B = par_map1(fabs, A, NULL, NULL);
		// Send the result back to A
		par_send(A, same, B, NULL, NULL);
	destroy_par_env();

	for(int i = 0; i < length(A); i++) {
		printf("%.2f ", VAL(A.a[i])); 
	}

	printf("\n");

	return 0;
}
