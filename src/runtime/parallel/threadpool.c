#include "threading.h"

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>

#include "../runtime.h"
#include "../parallel.h"

// Instructions to be used each time a thread in the threadpool is activated 
typedef struct _instruction {
	void (*work)(distribution dist, int id, par_array* out, void* f, void* p, void* args);	// Work to be executed on each thread
	void* f;				// Optional function
	void* p;				// Optional predicate
	void* args;
	distribution dist;
	par_array* output;
} instruction;

typedef struct _threadinfo {
	int 		id;
	pthread_t 	thread;
	instruction 	instr;
	par_array	retval;
} threadinfo;

threadinfo thrds[NUM_THREADS];

int num_blocked = 	0;	// Amount of blocked threads (for barriers)
char kill_threads = 	0;	// Condition for destroying all active worker threads

pthread_cond_t barrier_cond;
pthread_mutex_t barrier_mutex;

instruction global_instruction;

void barrier() {
	pthread_mutex_lock(&barrier_mutex);
	num_blocked += 1;

	// Wait for worker threads and main thread
	if(num_blocked >= NUM_THREADS + 1) {
		num_blocked = 0;
		pthread_cond_broadcast(&barrier_cond);
	} 
	else {
		pthread_cond_wait(&barrier_cond, &barrier_mutex);
	}

	pthread_mutex_unlock(&barrier_mutex);
}

void* worker(void* args) {
	int 		id;
	instruction 	instr;

	//id = *(int*)args;
	id = (int)args;

	for(;;) {
		barrier();
		if(kill_threads == 1)
			break;

		// Fetch instruction
		instr = global_instruction;

		instr.work(instr.dist, id, instr.output, instr.f, instr.p, instr.args);
		barrier();
	}
	pthread_exit(NULL);
	return NULL;
}

void init_thread(int id, threadinfo* out_info) {
	int s;

	int arg = id;
	s = pthread_create(&(out_info->thread), NULL, worker, (void*)arg);
	if(s != 0) {
		// TODO: Handle this
		printf("Thread %d failed to start.\n", id);
	}

	out_info->id = id;
	//out_info->retval = (dist_ret*)calloc(1, sizeof(dist_ret));
}

void init_threadpool() {
	pthread_mutex_init(&barrier_mutex, NULL);
	pthread_cond_init(&barrier_cond, NULL);

	for(int i = 0; i < NUM_THREADS; i++) {
		init_thread(i, thrds + i);
	}
}

void kill_threadpool() {
	kill_threads = 1;
	barrier();

	for(int i = 0; i < NUM_THREADS; i++)
		pthread_join(thrds[i].thread, NULL);
}

par_array execute_in_parallel(void (*work)(distribution dist, int id, par_array* out, void* f, void* p, void* args), distribution dist, int out_m, int out_n, void* f, void* p, void* args) {

	par_array ret = mk_array(NULL, out_m, out_n);

	global_instruction.work = work;
	global_instruction.f 	= f;
	global_instruction.p	= p;
	global_instruction.args = args;
	global_instruction.dist = dist;
	global_instruction.output = &ret;

	// Barrier. Wait for all active workers to finish
	barrier();
	// Worker threads perform their
	// work here
	barrier();

	return ret;
}
