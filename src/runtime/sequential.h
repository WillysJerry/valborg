/*
 * sequential.h
 * Sequential reference implementations for the data-parallel primitives.
 */

#ifndef SEQUENTIAL_H
#define SEQUENTIAL_H

#include "runtime.h"

par_array seq_get(const par_array a, int (*f)(int i));
void seq_send(par_array a, int (*f)(int i), const par_array b);

par_array seq_concat(const par_array a, const par_array b);
par_array seq_select(const par_array a, int m, int n);

// Maybe we can use variable arguments instead of defining functions for each num of input arrays (or maybe not seeing how the function to apply (f) needs to take the same amount of arguments as there are arrays passed, maybe a compiler thing to fix later on?)
par_array seq_map1();
par_array seq_map2();
par_array seq_map3();

int seq_reduce();
par_array seq_scan();

void seq_mask();

#endif // SEQUENTIAL_H
