#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "../src/runtime/sequential.h"
#include "../src/runtime/runtime.h"

int foo(int i) {
	printf("FOO CALLED: %d\n", i);
	return 2;
}

int src(int i) {
	return (i + 1) % 6;
}

int main(int argc, char** argv) {
	int m = 0xbabeface;

	double arr[] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0 };

	par_array a = mk_array(arr, 2, 7);
	par_array b = seq_get(a, src);

	// Make sure index bounds are correct
	assert(a.m == 2 && a.n == 7 && 
		b.m == 2 && b.n == 7);

	// Make sure length is correct
	assert(length(a) == 6 && length(b) == 6);

	// Make sure elements are correct
	double epsilon = 0.0001;
	for(int i = 0; i < 6; i++) {
		assert(abs(a.a[i] - arr[i]) <= epsilon );
		assert(abs(b.a[i] - arr[(i + 1) % 6]) <= epsilon );
		printf("%f %f\n", a.a[i], b.a[i]); 
	}



	return 0;
}
