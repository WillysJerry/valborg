/*
 * parallel.h
 * Parallel implementations of the data-parallel primitives.
 */

#ifndef PARALLEL_H
#define PARALLEL_H

#include "runtime.h"
#include "parallel/threading.h"

// Macros for converting between (L)ocal and (G)lobal indices in array A
#define L2G(A, i) ( (A).m + (i) )
#define G2L(A, i) ( (i) - (A).m )

void init_par_env();
void destroy_par_env();

par_array par_get(const par_array a, int (*f)(int i), int (*p)(int i));

void par_send(par_array a, int (*f)(int i), const par_array b, int (*p)(int i));

// Concatenates arrays a and b, resulting in a new array c where length(c) = length(a) + length(b)
par_array par_concat(const par_array a, const par_array b);

// Returns the subarray a[m..n]
par_array par_select(const par_array a, int m, int n);

// Maybe we can use variable arguments instead of defining functions for each num of input arrays (or maybe not seeing how the function to apply (f) needs to take the same amount of arguments as there are arrays passed, maybe a compiler thing to fix later on?)
par_array par_map1(double (*f)(double x), const par_array a, int (*p)(int i));
par_array par_map2(double (*f)(double x, double y), const par_array a, const par_array b, int (*p)(int i));
par_array par_map3(double (*f)(double x, double y, double z), const par_array a, const par_array b, const par_array c, int (*p)(int i));

double par_reduce(double (*f)(double x, double y), const par_array a, int (*p)(int i));
par_array par_scan(double (*f)(double x, double y), const par_array a, int (*p)(int i));

// Returns a new array where the elements fulfill predicate p, should probably be changed somehow to be more useful.
par_array par_mask(int (*p)(double x), par_array a);

#endif // PARALLEL_H
