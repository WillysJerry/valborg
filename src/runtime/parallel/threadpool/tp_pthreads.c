#include "threadpool.h"

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>

#include "../../runtime.h"
#include "../../parallel.h"

pthread_t thrds[NUM_THREADS - 1];
pthread_barrier_t barrier_barrier;
instruction global_instruction;

char kill_threads = 0;	// Condition for destroying all active worker threads

void do_work(int id) {
	instruction* inst = &global_instruction;

	inst->work(inst->dist, id, inst->output, inst->f, inst->p, inst->args, inst->cmp);
}

void* worker(void* args) {
	int 		id;

	//id = *(int*)args;
	id = (int)args;

	for(;;) {
		pthread_barrier_wait(&barrier_barrier);

		if(kill_threads == 1)
			break;

		do_work(id);

		pthread_barrier_wait(&barrier_barrier);
	}
	pthread_exit(NULL);
	return NULL;
}

void init_thread(int id, pthread_t* thread) {
	int s;

	int arg = id;
	s = pthread_create(thread, NULL, worker, (void*)arg);
	if(s != 0) {
		fprintf(stderr, "Thread %d failed to start.\n", id);
		exit(1);
	}
}

void init_threadpool() {
	pthread_barrier_init(&barrier_barrier, NULL, NUM_THREADS);

	for(int i = 1; i < NUM_THREADS; i++) {
		init_thread(i, thrds + i - 1);
	}
}

void kill_threadpool() {
	kill_threads = 1;
	pthread_barrier_wait(&barrier_barrier);

	for(int i = 0; i < NUM_THREADS - 1; i++)
		pthread_join(thrds[i], NULL);

	pthread_barrier_destroy(&barrier_barrier);
}

void execute_in_parallel(void (*work)(distribution dist, int id, par_array* out, void* f, void* p, void* args, void* cmp), distribution dist, par_array* out, void* f, void* p, void* args, void* cmp) {

	global_instruction.work = work;
	global_instruction.f 	= f;
	global_instruction.p	= p;
	global_instruction.args = args;
	global_instruction.dist = dist;
	global_instruction.output = out;
	global_instruction.cmp = cmp;


	pthread_barrier_wait(&barrier_barrier);
	do_work(0);
	// Worker threads perform their
	// work here
	pthread_barrier_wait(&barrier_barrier);
}
