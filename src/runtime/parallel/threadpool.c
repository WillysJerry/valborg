#include "threading.h"

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>

#include <math.h>

#include "../runtime.h"
#include "../parallel.h"

// Instructions to be used each time a thread in the threadpool is activated 
typedef struct _instruction {
	int m, n;
	par_array* input;
	void* output;
	void (*work)(int i, par_array* input, void* output, void* f, void* p, void* args);	// Work to be executed on each thread
	void* f;				// Optional function
	void* p;				// Optional predicate
	void* args;
} instruction; 

typedef struct _threadinfo {
	int 		id;
	pthread_t 	thread;
	int		m, n;
} threadinfo;

threadinfo thrds[NUM_THREADS];

int num_blocked = 	0;	// Amount of blocked threads (for barriers)
char kill_threads = 	0;	// Condition for destroying all active worker threads

instruction current_instr;

pthread_cond_t barrier_cond;
pthread_mutex_t barrier_mutex;

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

void worker(void* args) {
	int 		id;
	instruction* 	instr;
	threadinfo*	info;

	id = (int)args;

	instr = &current_instr;
	info = 	thrds + id;

	for(;;) {
		barrier();
		if(kill_threads == 1)
			break;


		for(int i = info->m; i < info->n + 1; i++) {
			instr->work(i, instr->input, instr->output, instr->f, instr->p, instr->args);
		}
		barrier();
	}
	pthread_exit(NULL);
}


void init_thread(int id, threadinfo* out_info) {
	int s;

	s = pthread_create(&(out_info->thread), NULL, worker, (void*)(id));
	if(s != 0) {
		// TODO: Handle this
		printf("Thread %d failed to start.\n", id);
	}

	out_info->id = id;
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

void execute_in_parallel(void (*work)(int i, par_array* input, void* output, void* f, void* p, void* args), const par_array* input, void* output, int m, int n, void* f, void* p, void* args) {
	int len = n - m + 1;
	int blocksize = len / NUM_THREADS;
	int remainder = len % NUM_THREADS;

	int thrd_block;
	int acc = m;


	current_instr.m = acc;
	current_instr.n = acc + thrd_block - 1;
	current_instr.input = input;
	current_instr.output = output;
	current_instr.work = work;
	current_instr.f = f;
	current_instr.p = p;
	current_instr.args = args;
	for(int i = 0; i < NUM_THREADS; i++) {
		thrd_block = blocksize;
		if(i < remainder)
			thrd_block += 1;

		thrds[i].m = acc;
		thrds[i].n = acc + thrd_block - 1;
		// ouch
		/*thrds[i].instr.m = acc;
		thrds[i].instr.n = acc + thrd_block - 1;
		thrds[i].instr.input = input;
		thrds[i].instr.output = output;
		thrds[i].instr.work = work;
		thrds[i].instr.f = f;
		thrds[i].instr.p = p;
		thrds[i].instr.args = args;*/

		acc += thrd_block;
	}

	barrier();	// Wait until threads are ready to start executing instructions...
	// Threads do a bunch
	// of work here
	barrier();	// Sync threads, wait for them to finish...
}
