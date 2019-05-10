/*
 * sequential.h
 * Sequential reference implementations for the data-parallel primitives.
 */

#ifndef SEQUENTIAL_H
#define SEQUENTIAL_H

#include "../runtime.h"

par_array seq_get(const par_array a, int (*f)(int i), int (*p)(int i, const par_array x, void* cmp), void* cmp);

void seq_send(par_array a, int (*f)(int i), const par_array b, int (*p)(int i, const par_array lhs, const par_array rhs, void* cmp), void* cmp);

// Concatenates arrays a and b, resulting in a new array c where length(c) = length(a) + length(b)
par_array seq_concat(const par_array a, const par_array b);

// Returns the subarray a[m..n]
par_array seq_select(const par_array a, int m, int n, int (*p)(int i, const par_array x, void* cmp), void* cmp);

// Maybe we can use variable arguments instead of defining functions for each num of input arrays (or maybe not seeing how the function to apply (f) needs to take the same amount of arguments as there are arrays passed, maybe a compiler thing to fix later on?)
par_array seq_map1(double (*f)(double x), const par_array a, int (*p)(int i, const par_array x, void* cmp), void* cmp);
par_array seq_map2(double (*f)(double x, double y), const par_array a, const par_array b, int (*p)(int i, const par_array x, const par_array y, void* cmp), void* cmp);
par_array seq_map3(double (*f)(double x, double y, double z), const par_array a, const par_array b, const par_array c, int (*p)(int i, const par_array x, const par_array y, const par_array z, void* cmp), void* cmp);

double seq_reduce(double (*f)(double x, double y), const par_array a, int (*p)(int i, const par_array x, void* cmp), void* cmp);
par_array seq_scan(double (*f)(double x, double y), const par_array a, int(*p)(int i, const par_array x, void* cmp), void* cmp);

int seq_count(const par_array a, int (*p)(int i, const par_array x, void* cmp), void* cmp);

par_array seq_asn(const par_array a, const par_array b, int (*p)(int i, const par_array lhs, const par_array rhs, void* cmp), void* cmp);

#endif // SEQUENTIAL_H
