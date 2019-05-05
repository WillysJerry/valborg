#include "threading.h"

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>

#include "../runtime.h"
#include "../parallel.h"

// Instructions to be used each time a thread in the threadpool is activated 
typedef struct _instruction {
	void (*work)(distribution dist, int id, dist_ret* retval, void* f, void* p, void* args);	// Work to be executed on each thread
	distribution dist;
	void* f;				// Optional function
	void* p;				// Optional predicate
	void* args;
} instruction; 

typedef struct _threadinfo {
	int 		id;
	pthread_t 	thread;
	instruction 	instr;
	dist_ret*	retval;
} threadinfo;

threadinfo thrds[NUM_THREADS];

int num_blocked = 	0;	// Amount of blocked threads (for barriers)
char kill_threads = 	0;	// Condition for destroying all active worker threads

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
	dist_ret*	retval;

	id = (int)args;

	instr = &(thrds[id].instr);
	retval = thrds[id].retval;

	for(;;) {
		barrier();
		if(kill_threads == 1)
			break;

		retval->n = 0;
		retval->v = NULL;
		instr->work(instr->dist, id, retval, instr->f, instr->p, instr->args);
		barrier();
	}
	free(retval);
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
	out_info->retval = (dist_ret*)calloc(1, sizeof(dist_ret));
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

dist_ret** execute_in_parallel(void (*work)(distribution dist, int id, dist_ret* retval, void* f, void* p), distribution dist, void* f, void* p, void* args) {
	dist_ret** ret = (dist_ret**)calloc(NUM_THREADS, sizeof(dist_ret*));
	int s = 0;

	for(int i = 0; i < NUM_THREADS; i++) {
		thrds[i].instr.work = work;
		thrds[i].instr.f = f;
		thrds[i].instr.p = p;
		thrds[i].instr.dist = dist;
		thrds[i].instr.args = args;
	}

	// Barrier. Wait for all active workers to finish
	barrier();
	for(int i = 0; i < NUM_THREADS; i++) {
		ret[i] = thrds[i].retval;
	}
	barrier();

	return ret;
}
