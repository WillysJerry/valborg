#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../src/runtime/sequential.h"
#include "../src/runtime/runtime.h"

int main(int argc, char** argv) {
	int m = 0xbabeface;

	int arr[] = { 1, 2, 3, 4, 5, 6 };

	par_array a = mk_int_array(arr, 2, 7);

	// Make sure index bounds are correct
	assert(a.m == 2 && a.n == 7);

	// Make sure length is correct
	assert(length(a) == 6);

	// Make sure elements are correct
	for(int i = 0; i < 6; i++) {
		// jag vill kräkas
		assert(a.a._i[i] == i + 1);
		printf("%d ", a.a._i[i]); 
	}

	char arr2[] = { 'a', 'b', 'c' };
	par_array b = mk_char_array(arr2, 0, 2);
	printf("\n%d\n%d\n", sizeof(*(b.a._c)), sizeof(*(a.a)));

	return 0;
}
