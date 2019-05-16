// Vector dot product 
//
// Input: 
// 	A = [1.0, 2.0, 3.0, 4.0, 5.0]
// 	B = [6.0, 7.0, 8.0, 9.0, 10.0]
// Do: 	
// 	dot(A, B)
// Alternatively:
// 	forall k do
// 		C[k] = A[k] * B[k]
// 	r = reduce(C) 
// Expected output: 
// 	130.0	

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#include <runtime.h>

double mul2(double x, double y) {
	return x * y;
}

double sum2(double x, double y) {
	return x + y;
}
int main(int argc, char** argv) {

	float res;

	double arr0[] = { 1.0, 2.0, 3.0, 4.0, 5.0 };
	double arr1[] = { 6.0, 7.0, 8.0, 9.0, 10.0 };

	par_array A = mk_array(arr0, 0, 4);
	par_array B = mk_array(arr1, 0, 4);

	vb_init_par_env();	
		// Multiply A and B element-wise
		par_array C = vb_map2( mul2, A, B, NULL, NULL );
		// Reduce the result of the element-wise multiplication to get the final dot product
		res = vb_reduce(sum2, C, NULL, NULL);
	vb_destroy_par_env();

	printf("%.2f ", res); 

	printf("\n");

	return 0;
}
