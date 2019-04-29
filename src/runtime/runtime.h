/*
 * runtime.h
 * Interface shared between both the parallel and sequential runtime
 */
#ifndef RUNTIME_H
#define RUNTIME_H

// Temporary solution for "polymorphic" array types: a parallel array can only contain the types specified in the union below, maybe change this to a void pointer or something later on.
// Problem with this: How can we know the size of the elements in the array when freeing the memory if using a union for pointers (i.e a char takes up 1 byte while an int takes up 4, how to tell the difference with unions)?
typedef union {
	char* 	_c;
	int* 	_i;
	float* 	_f;
} p_u;

typedef struct _par_array {
	int 	m, n; 	// Lower and upper bounds of the array
	p_u 	a; 	// Pointer to the actual array in memory
} par_array;

// Creates a parallel array from input array a, lower index bound n and upper index bound m
// Maybe replace this with something less annoying
par_array mk_char_array(const char* a, int m, int n);
par_array mk_int_array(const int* a, int m, int n);
par_array mk_float_array(const float* a, int m, int n);

// Returns the length of a
int length(const par_array a); 

#endif // RUNTIME_H
