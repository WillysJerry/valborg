/*
 * sequential.h
 * Sequential reference implementations for the data-parallel primitives.
 */

#ifndef SEQUENTIAL_H
#define SEQUENTIAL_H

#include "runtime.h"

par_array seq_get(const par_array a, int (*f)(int i), int (*p)(int i, double x));

void seq_send(par_array a, int (*f)(int i), const par_array b, int (*p)(int i, double lhs, double rhs));

// Concatenates arrays a and b, resulting in a new array c where length(c) = length(a) + length(b)
par_array seq_concat(const par_array a, const par_array b);

// Returns the subarray a[m..n]
par_array seq_select(const par_array a, int m, int n, int (*p)(int i, double x));

// Maybe we can use variable arguments instead of defining functions for each num of input arrays (or maybe not seeing how the function to apply (f) needs to take the same amount of arguments as there are arrays passed, maybe a compiler thing to fix later on?)
par_array seq_map1(double (*f)(double x), const par_array a, int (*p)(int i, double x));
par_array seq_map2(double (*f)(double x, double y), const par_array a, const par_array b, int (*p)(int i, double x, double y));
par_array seq_map3(double (*f)(double x, double y, double z), const par_array a, const par_array b, const par_array c, int (*p)(int i, double x, double y, double z));

double seq_reduce(double (*f)(double x, double y), const par_array a, int (*p)(int i, double x));
par_array seq_scan(double (*f)(double x, double y), const par_array a, int(*p)(int i, double x));

int seq_count(const par_array a, int (*p)(int i, double x));

par_array seq_asn(const par_array a, const par_array b, int (*p)(int i, double lhs, double rhs));

#endif // SEQUENTIAL_H
