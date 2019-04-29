/*
 * sequential.h
 * Sequential reference implementations for the data-parallel primitives.
 */

#ifndef SEQUENTIAL_H
#define SEQUENTIAL_H

#include "runtime.h"

void seq_send(par_array a, int (*f)(int i), const par_array b);

par_array seq_concat(const par_array a, const par_array b);
par_array seq_select(const par_array a, int m, int n);

// Maybe we can use variable arguments instead of defining functions for each num of input arrays
par_array seq_map1();
par_array seq_map2();
par_array seq_map3();

int seq_reduce();
par_array seq_scan();

void seq_mask();

#endif // SEQUENTIAL_H
