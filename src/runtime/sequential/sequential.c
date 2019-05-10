#include "sequential.h"

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
par_array seq_get(const par_array a, int (*f)(int i), int (*p)(int i, const par_array x, void* cmp), void* cmp) {
	// TODO: A good idea might be to set the bounds (m..n) of the output array dynamically based on the mapping function,
	// as this might allow for easy shifting of arrays by for instance mapping i->(i+1), which would mean that the output m
	// should be m+1 and the out n should be n+1 (rather then keeping them the same as the in array). 

	// Allocate temp array
	par_array b = mk_array(NULL, a.m, a.n);
	int src_i;

	for(int i = a.m; i < a.n + 1; i++) {
		src_i = G2L(a, f(i));
		if(SATISFIES(p, src_i, a, cmp)) {
			memcpy(	b.a + G2L(b, i),
				a.a + G2L(a, src_i),
				sizeof(maybe));
		}
	}

	return b;
}

// This function has side-effects. Possible race condition if f(i) == f(j) for some i and j.
// Another thing to consider is how the bounds of the two arrays should work together, maybe we only send the elements that are intersecting? For now this is ignored.
// The source function maps GLOBAL SPACE indices.
void seq_send(par_array a, int (*f)(int i), const par_array b, int (*p)(int i, const par_array lhs, const par_array rhs, void* cmp), void* cmp) {
	int i;
	int dst_i, src_i;

	for(i = a.m; i < a.n + 1; i++) {
		dst_i = G2L(a, f(i));
		src_i = G2L(b, i);
		if(SATISFIES(p, i, a, b, cmp)) {
			// Need to convert from global to local index space
			a.a[dst_i] = b.a[src_i];
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

par_array seq_select(const par_array a, int m, int n, int (*p)(int i, const par_array x, void* cmp), void* cmp) {
	par_array res = mk_array(NULL, m, n);

	for(int i = m; i < n + 1; i++) {
		if(SATISFIES(p, i, a, cmp)) {
			res.a[G2L(res, i)] = a.a[G2L(a, i)];
		} else {
			res.a[G2L(res, i)] = NONE;
		}
	}
	return res;
}

par_array seq_map1(double (*f)(double x), const par_array a, int (*p)(int i, const par_array x, void* cmp), void* cmp) {
	int i, j;
	par_array arr = mk_array(NULL, a.m, a.n);

	maybe x;

	// i is a "global" index (i.e something between m..n)
	// j is a "local" index (i.e something from 0..something)
	for(i = a.m, j = 0 ; i < a.n + 1; i++, j++) {
		x = a.a[G2L(a, i)];
		if(IS_SOME(x) && 
		    SATISFIES(p, i, a, cmp)) {

			arr.a[j] = SOME( f(VAL(x)) );
		} else {
			arr.a[j] = NONE;
		}
	}

	return arr;
}

// How to handle these (map2, map3)? Should arrays of different sizes be allowed? Also how will it work with index bounds?
par_array seq_map2(double (*f)(double x, double y), const par_array a, const par_array b, int (*p)(int i, const par_array x, const par_array y, void* cmp), void* cmp) {
	int i, j;
	const par_array arrs[] = {a, b};
	bounds rng = intersection( arrs, 2 );

	par_array arr = mk_array(NULL, rng.m, rng.n);

	maybe x, y;

	// i is a "global" index (i.e something between m..n)
	// j is a "local" index (i.e something from 0..something)
	for(i = rng.m, j = 0 ; i < rng.n + 1; i++, j++) {
		x = a.a[G2L(a, i)];
		y = b.a[G2L(b, i)];
		if(IS_SOME(x)  && 
		    IS_SOME(y) &&
		    SATISFIES(p, i, a, b, cmp)) {

			arr.a[j] = SOME(f(	VAL(x), 
						VAL(y)));
		} else {
			arr.a[j] = NONE;
		}
	}

	return arr;
}

par_array seq_map3(double (*f)(double x, double y, double z), const par_array a, const par_array b, const par_array c, int (*p)(int i, const par_array x, const par_array y, const par_array z, void* cmp), void* cmp) {
	int i, j;
	const par_array arrs[] = {a, b, c};
	bounds rng = intersection( arrs, 3 );
	par_array arr = mk_array(NULL, rng.m, rng.n);

	maybe x, y, z;

	// i is a "global" index (i.e something between m..n)
	// j is a "local" index (i.e something from 0..something)
	for(i = rng.m, j = 0 ; i < rng.n + 1; i++, j++) {
		x = a.a[G2L(a, i)];
		y = b.a[G2L(b, i)];
		z = c.a[G2L(c, i)];

		if(IS_SOME(x) && 
		    IS_SOME(y) && 
		    IS_SOME(z) &&
		    SATISFIES(p, i, a, b, c, cmp)) {

			arr.a[j] = SOME(f(VAL(x), VAL(y), VAL(z))); 
		} else {
			arr.a[j] = NONE;
		}
	}

	return arr;
}

// Sequential reduce and scan are basically linear pairwise applications of the function f over the input array
double seq_reduce(double (*f)(double x, double y), const par_array a, int (*p)(int i, const par_array x, void* cmp), void* cmp) {
	int i;
	double res = 0.0;
	for(i = 0; i < length(a); i++) {
		if(IS_SOME(a.a[i]) && 
		    SATISFIES(p, L2G(a, i), a, cmp)) {

			res = VAL(a.a[i]);
			i++;
			break;
		}
	}
	for(; i < length(a); i++) {
		if(IS_SOME(a.a[i]) && 
		    SATISFIES(p, L2G(a, i), a, cmp)) {

				res = f(res, VAL(a.a[i]));	
		}
	}

	return res;
}

par_array seq_scan(double (*f)(double x, double y), const par_array a, int(*p)(int i, const par_array x, void* cmp), void* cmp) {
	int i;
	par_array arr = mk_array(NULL, a.m, a.n);
	double acc = 0.0;

	// a[0], f(a[0], a[1]), f(a[0], f(a[1], a[2])), ...
	
	// Find starting index (to which we don't apply f)
	for(i = 0; i < length(a); i++) {
		if(IS_SOME(a.a[i]) &&
		    SATISFIES(p, L2G(a, i), a, cmp)) {
			acc = VAL(a.a[i]);
			arr.a[i] = SOME(acc);
			i++;
			break;
		}	
	}

	// Scan through the remaining indices of the array
	for(; i < length(a); i++) {
		if(IS_SOME(a.a[i]) &&
		    SATISFIES(p, L2G(a, i), a, cmp)) {
			acc = f(acc, VAL(a.a[i]));
		}
		arr.a[i] = SOME(acc);
	}

	return arr;
}

int seq_count(const par_array a, int (*p)(int i, const par_array x, void* cmp), void* cmp) {
	int c = 0;
	for(int i = 0; i < length(a); i++) {
		if(IS_SOME(a.a[i]) && SATISFIES(p, i, a, cmp)) {
			c += 1;
		}
	}

	return c;
}

par_array seq_asn(const par_array a, const par_array b, int (*p)(int i, const par_array lhs, const par_array rhs, void* cmp), void* cmp) {
	if(a.m != b.m || a.n != b.n) {
		return mk_array(NULL, 0, -1);
	}
	par_array res = mk_array(NULL, a.m, a.n);

	for(int i = res.m; i < res.n + 1; i++) {
		if(SATISFIES(p, i, a, b, cmp)) {
			res.a[G2L(res, i)] = b.a[G2L(b, i)];
		} else {
			res.a[G2L(res, i)] = a.a[G2L(a, i)];
		}
	}

	return res;
}
