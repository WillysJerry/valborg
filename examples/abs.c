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

#include <runtime.h>

int lt0(int i, const par_array A, void* cmp) {
	return VAL(ELEM(A, i)) < 0.0;
}

int same(int i) {
	return i;
}

int main(int argc, char** argv) {

	double arr0[] = { 1.0, -2.0, 3.0, 4.0, -5.0 };

	par_array A = mk_array(arr0, 0, 4);

	vb_init_par_env();
		// Apply fabs (abs for floating point values) elementwise on every element in A
		par_array B = vb_map1(fabs, A, lt0, NULL);
		// Send the result back to A
		vb_send(A, same, B, NULL, NULL);
	vb_destroy_par_env();

	for(int i = 0; i < length(A); i++) {
		printf("%.2f ", VAL(A.a[i])); 
	}

	printf("\n");

	return 0;
}
