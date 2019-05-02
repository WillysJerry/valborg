#ifndef BENCHMARK_H
#define BENCHMARK_H

#include<stdio.h>
#include<sys/time.h>

double get_time_usec() {
	static struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000.0 + tv.tv_usec;
}

double get_timediff(double t0, double t1) {
	return t1 - t0;
}

void log_benchmark(double seq_t, double par_t, int n_elements, const char* test_name) {
	printf("Execution of %s over %d elements finished.\nSequential execution time: %.1lf microseconds.\nParallel execution time: %.1lf microseconds.\nSpeedup: %.1lf.\n", test_name, n_elements, seq_t, par_t, seq_t / par_t);

}

#endif // BENCHMARK_H
