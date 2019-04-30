/*
 * runtime.h
 * Interface shared between both the parallel and sequential runtime
 */
#ifndef RUNTIME_H
#define RUNTIME_H


typedef struct _par_array {
	int 	m, n; 	// Lower and upper bounds of the array
	double*	a;	// Pointer to the actual array in memory
} par_array;

// Creates a parallel array from input array a, lower index bound n and upper index bound m
// Maybe replace this with something less annoying
par_array mk_array(const double* a, int m, int n);

// Returns the length of a
int length(const par_array a); 

#endif // RUNTIME_H
