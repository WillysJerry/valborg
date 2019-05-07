#include "../sequential.h"
#include "../runtime.h"

#include <stdlib.h>
#include <string.h> // For memcpy

// 'p' is a pointer to a predicate used to mask out certain elements. For now the values of all masked element will be 0, which may or may not be a problem depending on how we want the out arrays to look. The predicate can also be NULL which makes this function perform a normal get communication.
// I see two possible ways to tackle masking, either we "squish" the resulting array from masked values, resulting in a smaller array:
// ------------------
// A:  3  4 -1  5 -6  9     "forall i where i>0 do B[i] = A[i] }"
//     |  |   _/ ____/
//     v  v  v  v
// B:  3  4  5  9
// ------------------
// Or we 0 the masked elements instead, creating a sparse array of the same size as the input array (this solution probably makes more sense in regards to the "source" function f):
// ------------------
// A:  3  4 -1  5 -6  9     "forall i where i>0 do B[i] = A[i]"
//     |  |  |  |  |  |
//     v  v  v  v  v  v
// B:  3  4  0  5  0  9
// ------------------
par_array seq_get(const par_array a, int (*f)(int i), int (*p)(int i)) {
	// TODO: A good idea might be to set the bounds (m..n) of the output array dynamically based on the mapping function,
	// as this might allow for easy shifting of arrays by for instance mapping i->(i+1), which would mean that the output m
	// should be m+1 and the out n should be n+1 (rather then keeping them the same as the in array). 

	// Allocate temp array
	par_array b = mk_array(NULL, a.m, a.n);
	//maybe* arr = (maybe*)calloc(len, sizeof(maybe));

	for(int i = a.m; i < a.n + 1; i++) {
		if(p == NULL || p(i)) {
			memcpy(	b.a + G2L(b, i),
				a.a + G2L(a, f(i)),
				sizeof(maybe));
		}
	}

	//par_array b = mk_array(arr, a.m, a.n); 
	//free(arr);

	return b;
}

// This function has side-effects. Possible race condition if f(i) == f(j) for some i and j.
// Another thing to consider is how the bounds of the two arrays should work together, maybe we only send the elements that are intersecting? For now this is ignored.
// The source function maps GLOBAL SPACE indices.
void seq_send(par_array a, int (*f)(int i), const par_array b, int (*p)(int i)) {
	int i;

	for(i = a.m; i < a.n + 1; i++) {
		if(p == NULL || p(i)) {
			// Need to convert from global to local index space
			a.a[G2L(a, i)] = b.a[G2L(b, f(i))];
		}
	}	
}

par_array seq_concat(const par_array a, const par_array b) {
	int len = length(a) + length(b);

	par_array c = mk_array(NULL, a.m, a.m + len - 1);
	memcpy(c.a, a.a, sizeof(maybe) * length(a));
	memcpy(c.a + length(a), b.a, sizeof(maybe) * length(b));

	return c;
}

par_array seq_select(const par_array a, int m, int n) {
	//par_array b = clone_array(a.a + G2L(a, m), m, n);
	

	//return b;
	
	return clone_array(a, m, n);
}

par_array seq_map1(double (*f)(double x), const par_array a, int (*p)(int i)) {
	int i, j;
	par_array arr = mk_array(NULL, a.m, a.n);

	for(i = a.m, j = 0 ; i < a.n + 1; i++, j++) {
		if(IS_SOME(a.a[G2L(a, i)])) {
			// i is a "global" index (i.e something between m..n)
			// j is a "local" index (i.e something from 0..something)
			if(p == NULL || p(i)) {
				arr.a[j] = SOME( f(VAL(a.a[G2L(a, i)])) );
			}
		} else {
			arr.a[j] = NONE;
		}
	}

	return arr;
}

// How to handle these (map2, map3)? Should arrays of different sizes be allowed? Also how will it work with index bounds?
par_array seq_map2(double (*f)(double x, double y), const par_array a, const par_array b, int (*p)(int i)) {
	int i, j;
	const par_array arrs[] = {a, b};
	bounds rng = intersection( arrs, 2 );

	par_array arr = mk_array(NULL, rng.m, rng.n);

	for(i = rng.m, j = 0 ; i < rng.n + 1; i++, j++) {
		if(IS_SOME(a.a[G2L(a, i)] ) && IS_SOME(b.a[G2L(a, i)])) {
			// i is a "global" index (i.e something between m..n)
			// j is a "local" index (i.e something from 0..something)
			if(p == NULL || p(i)) {
				arr.a[j] = SOME(f(	VAL(a.a[G2L(a, i)]), 
							VAL(b.a[G2L(b, i)])));
			}
		} else {
			arr.a[j] = NONE;
		}
	}

	return arr;
}

par_array seq_map3(double (*f)(double x, double y, double z), const par_array a, const par_array b, const par_array c, int (*p)(int i)) {
	int i, j;
	const par_array arrs[] = {a, b, c};
	bounds rng = intersection( arrs, 3 );
	par_array arr = mk_array(NULL, rng.m, rng.n);

	for(i = rng.m, j = 0 ; i < rng.n + 1; i++, j++) {
		if(IS_SOME(a.a[G2L(a, i)] ) && IS_SOME(b.a[G2L(a, i)]) && IS_SOME(c.a[G2L(a, i)])) {
			// i is a "global" index (i.e something between m..n)
			// j is a "local" index (i.e something from 0..something)
			if(p == NULL || p(i)) {
				arr.a[j] = SOME(f(	VAL(a.a[G2L(a, i)]), 
							VAL(b.a[G2L(b, i)]), 
							VAL(c.a[G2L(c, i)])));
			}
		} else {
			arr.a[j] = NONE;
		}
	}

	return arr;
}

// Sequential reduce and scan are basically linear pairwise applications of the function f over the input array
double seq_reduce(double (*f)(double x, double y), const par_array a, int (*p)(int i)) {
	int i;
	double res = 0.0;
	for(i = 0; i < length(a); i++) {
		if(IS_SOME(a.a[i])) {
			if(p == NULL || p(L2G(a, i))) {
				res = VAL(a.a[i]);
				i++;
				break;
			}
		}
	}
	for(; i < length(a); i++) {
		if(IS_SOME(a.a[i])) {
			if(p == NULL || p(L2G(a, i)))
				res = f(res, VAL(a.a[i]));	
		}
	}

	return res;
}

par_array seq_scan(double (*f)(double x, double y), const par_array a, int (*p)(int i)) {
	int i;
	par_array arr = mk_array(NULL, a.m, a.n);
	double acc = 0.0;

	// a[0], f(a[0], a[1]), f(a[0], f(a[1], a[2])), ...
	
	// Find starting index (to which we don't apply f)
	for(i = 0; i < length(a); i++) {
		if(IS_SOME(a.a[i])) {
			if(p == NULL || p(local_to_global(a, i))) {
				acc = VAL(a.a[i]);
				arr.a[i] = SOME(acc);
				i++;
				break;
			}	
		}
	}

	// Scan through the remaining indices of the array
	for(; i < length(a); i++) {
		if(IS_SOME(a.a[i])) {
			if(p == NULL || p(local_to_global(a, i))) {
				acc = f(acc, VAL(a.a[i]));
			}
			arr.a[i] = SOME(acc);
		} else {
			arr.a[i] = SOME(acc);
		}
	}

	return arr;
}

