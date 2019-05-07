/*
 * runtime.h
 * Interface shared between both the parallel and sequential runtime
 */
#ifndef RUNTIME_H
#define RUNTIME_H

typedef struct _bounds {
	int m, n;
} bounds;

typedef struct _par_array {
	int 	m, n; 	// Lower and upper bounds of the array
	double*	a;	// Pointer to the actual array in memory
} par_array;

// Creates a parallel array from input array a, lower index bound n and upper index bound m
// Maybe replace this with something less annoying
par_array mk_array(const double* a, int m, int n);

// Returns the length of a
int length(const par_array a); 

// Functions for converting global indices to local and vice-versa
int global_to_local(const par_array a, int i);
int local_to_global(const par_array a, int i);

void par_array_set(const par_array a, int gi, double v);
double par_array_get(const par_array a, int gi);

// Returns the set of intersecting indices between arrays 1..n
bounds intersection(const par_array* arrs, int n);

#endif // RUNTIME_H
