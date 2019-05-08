#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "../src/runtime/sequential.h"
#include "../src/runtime/parallel.h"
#include "../src/runtime/runtime.h"

int lt(int i, double x) {
}

par_array qsort(par_array in) {
	if(par_count(in, NULL) < 2) {
		return in;
	}

	// We assume that the array is dense for now 
	// (if it is not we need to make sure that the pivot is SOME)
	double pivot = in.a[0];
	
	par_array less = par_select(in, in.m, in.n,  

}

int main(int argc, char** argv) {
	par_array R; 	// Resulting array

	init_par_env();

	destroy_par_env();
}
