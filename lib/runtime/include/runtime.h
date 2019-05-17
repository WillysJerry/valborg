/*
 * runtime.h
 * Interface shared between both the parallel and sequential runtime
 */
#ifndef RUNTIME_H
#define RUNTIME_H

#define SOME(X) (struct _maybe){ .val=X, 	.active=1 }
#define NONE	(struct _maybe){ .val=0.0, 	.active=0 }

#define IS_SOME(X) ((X).active == 1)
#define IS_NONE(X) ((X).active == 0)

// Might want to change how we handle getting values from NONEs
#define VAL(X) ( ((X).active == 1) ? (X).val : 0.0 )

// Macros for converting between (L)ocal and (G)lobal indices in array A
#define L2G(A, i) ( (A).m + (i) )
#define G2L(A, i) ( (i) - (A).m )

// Macro for predicate checking
#define SATISFIES(P, ...) ( ((P) == NULL || (P(__VA_ARGS__)) ) )

#define ELEM(P, I) ((P).a[G2L( (P), (I) )])

typedef struct _bounds {
	int m, n;
} bounds;

typedef struct _maybe {
	double	val;
	unsigned char active;
} maybe;

typedef struct _par_array {
	int 	m, n; 	// Lower and upper bounds of the array
	maybe*	a;	// Pointer to the actual array in memory
} par_array;

// Creates a parallel array from input array a, lower index bound n and upper index bound m
// Maybe replace this with something less annoying
par_array mk_array(const double* a, int m, int n);

// Creates a copy of array 'a' with indices m..n 
par_array clone_array(const par_array a, int m, int n);

// Returns the length of a
int length(const par_array a); 

// Functions for converting global indices to local and vice-versa
int global_to_local(const par_array a, int i);
int local_to_global(const par_array a, int i);

// Returns the set of intersecting indices between arrays 1..n
bounds intersection(const par_array* arrs, int n);

void vb_init_par_env();
void vb_destroy_par_env();

par_array vb_get(const par_array a, int (*f)(int i), int (*p)(int i, const par_array x, void* cmp), void* cmp);

void vb_send(par_array a, int (*f)(int i), const par_array b, int (*p)(int i, const par_array lhs, const par_array rhs, void* cmp), void* cmp);

// Concatenates arrays a and b, resulting in a new array c where length(c) = length(a) + length(b)
par_array vb_concat(const par_array a, const par_array b);

// Returns the subarray a[m..n]
par_array vb_select(const par_array a, int m, int n, int (*p)(int i, const par_array x, void* cmp), void* cmp);

// par_array we can use variable arguments instead of defining functions for each num of input arrays (or par_array not seeing how the function to apply (f) needs to take the same amount of arguments as there are arrays passed, par_array a compiler thing to fix later on?)
par_array vb_map1(double (*f)(double x), const par_array a, int (*p)(int i, const par_array x, void* cmp), void* cmp);
par_array vb_map2(double (*f)(double x, double y), const par_array a, const par_array b, int (*p)(int i, const par_array x, const par_array y, void* cmp), void* cmp);
par_array vb_map3(double (*f)(double x, double y, double z), const par_array a, const par_array b, const par_array c, int (*p)(int i, const par_array x, const par_array y, const par_array z, void* cmp), void* cmp);

double vb_reduce(double (*f)(double x, double y), double v, const par_array a, int (*p)(int i, const par_array x, void* cmp), void* cmp);
par_array vb_scan(double (*f)(double x, double y), double v, const par_array a, int (*p)(int i, const par_array x, void* cmp), void* cmp);


// Returns the number of elements that are SOME and that satisfies predicate p
int vb_count(const par_array a, int (*p)(int i, const par_array x, void* cmp), void* cmp);

par_array vb_asn(const par_array a, const par_array b, int (*p)(int i, const par_array lhs, const par_array rhs, void* cmp), void* cmp);

par_array vb_replicate(double v, int m, int n, int (*p)(int i, void* cmp), void* cmp);

#endif // RUNTIME_H
