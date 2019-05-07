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

#endif // RUNTIME_H
