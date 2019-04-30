#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "../src/runtime/sequential.h"
#include "../src/runtime/runtime.h"

#define EPSILON 0.0001

// Bunch of functions that are sent to the data-parallel functions during tests
double square(double x) {
	return x*x;
}
double sum2(double x, double y) {
	return x+y;
}
double sum3(double x, double y, double z) {
	return x+y+z;
}
int lt3(double x) {
	return x<3;
}

// Mapping function used for get and send communication. Remember to update tests if this function is changed or things will be scuffed.
int src(int i) {
	return (i + 1) % 6;
}


void test_get(const par_array A) {
	par_array B = seq_get(A, src, NULL);

	assert(B.m == A.m && B.n == A.n);
	assert(length(A) == 6 && length(B) == 6);

	printf("#### Testing GET ####\n");
	for(int i = 0; i < length(B); i++) {
		// B should be A shifted one step to the right
		assert(abs(B.a[i] - A.a[(i + 1) % 6]) <= EPSILON);
		printf("A[%d]->B[%d]: %.1f\n", (i+1)%length(A), i, B.a[i]); 
	}

	free(B.a);

	// Make sure masking works
	B = seq_get(A, src, lt3);
	for(int i = 0; i < length(B); i++) {
		assert(lt3(B.a[i]));
		printf("A[%d]->B[%d] where A[%d] < 3: %.1f\n", (i+1)%length(A), i, (i+1)%length(A), B.a[i]);
	}

	free(B.a);
}

void test_send(const par_array A) {
	par_array B = mk_array(NULL, 2, 7);
	seq_send(B, src, A);

	printf("#### Testing SEND ####\n");
	for(int i = 0; i < 6; i++) {
		// B should be A shifted one step to the right
		assert(abs(B.a[i] - A.a[(i + 1) % 6]) <= EPSILON);
		printf("B[%d]<-A[%d]: %.1f\n", i, (i+1)%length(A), B.a[i]);
	}

}

void test_concat(const par_array A) {
	int i;
	double arr[] = { 7.0, 8.0, 9.0, 10.0 };
	par_array B = mk_array(arr, 0, 3);

	par_array C = seq_concat(A, B);

	assert(length(C) == length(A) + length(B));

	printf("#### Testing CONCAT ####\n");
	printf("B: ");
	for(i = 0; i < length(B); i++) {
		printf("%.1f ", B.a[i]);
	}

	printf("\nA@B: ");
	for(i = 0; i < length(A); i++) {
		assert(C.a[i] == A.a[i]);
		printf("%.1f ", C.a[i]);	
	}
	for(; i < length(C); i++) {
		assert(C.a[i] == B.a[i-length(A)]);
		printf("%.1f ", C.a[i]);	
	}

	free(C.a);

	C = seq_concat(B, A);
	printf("\nB@A: ");
	for(i = 0; i < length(B); i++) {
		assert(C.a[i] == B.a[i]);
		printf("%.1f ", C.a[i]);	
	}
	for(; i < length(C); i++) {
		assert(C.a[i] == A.a[i-length(B)]);
		printf("%.1f ", C.a[i]);	
	}
	printf("\n");

	free(B.a);
	free(C.a);
}

void test_select(const par_array A, int m, int n) {
	par_array B = seq_select(A, m, n);

	printf("#### Testing SELECT ####\n");
	for(int i = 0; i < length(B); i++) {
		assert(B.a[i] == A.a[i + m]);
		printf("A{%d..%d}[%d]: %.1f\n", m, n, i, B.a[i]);
	}

	free(B.a);
}

void test_map1(const par_array A) {
	par_array B = seq_map1(square, A);
	printf("#### Test MAP1 ####\n");
	printf("map square A\n");
	for(int i = 0; i < length(A); i++) {
		assert(
			abs(B.a[i] - square(A.a[i]))
			<= EPSILON);
		printf("%.1f ", B.a[i]);
	}
	printf("\n");

}

void test_map2(const par_array A) {
	par_array B = seq_map2(sum2, A, A);
	printf("#### Test MAP2 ####\n");
	printf("map sum2 A A\n");
	for(int i = 0; i < length(A); i++) {
		assert(
			abs(B.a[i] - 2 * A.a[i])
			<= EPSILON);
		printf("%.1f ", B.a[i]);
	}
	printf("\n");

}

void test_map3(const par_array A) {
	par_array B = seq_map3(sum3, A, A, A);
	printf("#### Test MAP3 ####\n");
	printf("map sum3 A A A\n");
	for(int i = 0; i < length(A); i++) {
		assert(
			abs(B.a[i] - 3 * A.a[i])
			<= EPSILON);
		printf("%.1f ", B.a[i]);
	}
	printf("\n");

}

void test_reduce(const par_array A) {
	double red = seq_reduce(sum2, A);	
	double acc = 0.0;
	printf("#### Testing REDUCE ####\n");
	for(int i = 0; i < length(A); i++) {
		acc += A.a[i];
	}
	assert(red == acc);
	printf("Summation of A: %.1f\n", red);
}

void test_scan(const par_array A) {
	par_array scan = seq_scan(sum2, A);	
	double acc = 0.0;
	printf("#### Testing SCAN ####\n");
	for(int i = 0; i < length(A); i++) {
		acc += A.a[i];
		printf("SCAN(A)[%d]: %.1f\n", i, scan.a[i]);
	}
}

int main(int argc, char** argv) {
	int m = 0xbabeface;
	int n = 0xbeefcab;

	double arr[] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0 };

	// Never change A or shit wont work
	const par_array A = mk_array(arr, 2, 7);

	// Initial A checks
	assert(A.m == 2 && A.n == 7);
	assert(length(A) == 6);
	for(int i = 0; i < 6; i++) {
		assert(abs(A.a[i] - arr[i]) <= EPSILON);
		printf("A[%d]: %.1f\n", i, A.a[i]);
	}

	// Test data-parallel functions
	test_get(A);
	test_send(A);
	test_concat(A);
	test_select(A, 1, 3);
	test_map1(A);
	test_map2(A);
	test_map3(A);
	test_reduce(A);
	test_scan(A);

	return 0;
}
