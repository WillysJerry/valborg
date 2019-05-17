// FIR-filter example 
//
// Input: 
// 	X = some input time series (parallel array)
// 	W = some array of weights 
// Do: 	
//	Y = replicate(0.0, X.m, length(X) - length(W))
// 	forall i in W do
// 		forall j in Y in parallel do
//	 		Y[j] = Y[j] + W[i] * X[j+1]

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
int slide(int i, void* arg) {
	int s = *(int*)arg;
	return i + s;
}
int main(int argc, char** argv) {
	// Input time series
	double arr0[] = { 1.0, 5.0, 2.0, -2.0, 3.0, 6.0, 0.0, -3.0, 1.0, 5.0, 2.0, -1.0, -6.0 };
	par_array A = mk_array(arr0, 0, 12);

	// Non-parallel array of k weights
	double weights[] = { 0.3, 0.5, 1.0 };
	int k = 3;


	vb_init_par_env();	
		// For each weight...
		par_array Y = vb_replicate(0.0, A.m, A.n - k + 1, NULL, NULL);
		par_array prev;
		for(int i = 0; i < k; i++) {
			par_array W_i = vb_replicate(weights[i], A.m, A.n, NULL, NULL);
			par_array X_i = vb_get(A, slide, (void*)&i, NULL, NULL);
			par_array B = vb_map2(mul2, W_i, X_i, NULL, NULL);
			prev = Y;
			Y = vb_map2(sum2, B, Y, NULL, NULL);

			free(W_i.a);
			free(X_i.a);
			free(B.a);
			free(prev.a);
		}
	vb_destroy_par_env();

	for(int i = 0; i < length(Y); i++) {
		printf("%.2f ", VAL(Y.a[i])); 
	}

	printf("\n");

	return 0;
}
