// Vector-Scalar Multiplication
//
// Input: 
// 	A = [1.0, 2.0, 3.0, 4.0, 5.0]
// Do: 	
// 	forall k where k%2==1 do A[k] * 5.0 
// Expected output: 
// 	[1.0, 10.0, 3.0, 20.0, 5.0]

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#include <runtime.h>

int repl_is_odd(int i, void* cmp) {
	return i % 2 == 1;
}

int send_is_odd(int i, const par_array lhs, const par_array rhs, void* cmp) {
	return i % 2 == 1;
}

double mul2(double x, double y) {
	return x*y;
}

int same(int i, void* args) {
	return i;
}

int main(int argc, char** argv) {
	double arr0[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
	par_array A = mk_array(arr0, 0, 4);
	vb_init_par_env();
		// Replicate the scalar value to an array of the same size as A
		par_array B = vb_replicate(5.0, A.m, A.n, repl_is_odd, NULL);
		// Perform an elementwise multiplication of A and the replicated array
		par_array C = vb_map2(mul2, A, B, NULL, NULL);
		// Send the results back to A
		vb_send(A, same, NULL, C, send_is_odd, NULL);
	vb_destroy_par_env();

	for(int i = 0; i < length(A); i++) {
		printf("%.2f ", VAL(A.a[i])); 
	}

	printf("\n");
	
	return 0;
}
