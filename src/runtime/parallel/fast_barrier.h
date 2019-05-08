/*
 * Code courtesy of locklessinc.com ( https://locklessinc.com/articles/barriers/ )
 */


#include <pthread.h>
#include <linux/futex.h>
#include <stdint.h>
#include <limits.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/time.h>

typedef struct fast_barrier_t fast_barrier_t;
struct fast_barrier_t
{
	union
	{
		struct
		{
			unsigned seq;
			unsigned count;
		};
		unsigned long long reset;
	};
	unsigned refcount;
	unsigned total;
	int spins;
	unsigned flags;
};

void fast_barrier_init(fast_barrier_t* b, pthread_barrierattr_t* a, unsigned count);

void fast_barrier_destroy(fast_barrier_t* b); 

int fast_barrier_wait(fast_barrier_t *b);
