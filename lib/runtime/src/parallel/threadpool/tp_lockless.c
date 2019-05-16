#include "threadpool.h"

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>

pthread_t thrds[NUM_THREADS - 1];

volatile unsigned char barrier_buffer[NUM_THREADS - 1];
volatile char kill_threads;	// Condition for destroying all active worker threads
volatile int job_cycle;

instruction global_instruction;

/* Barrier synchronization called by the master thread */
void worker_sync(int id) {
	// The only thing we need to do is flag that this thread is ready to proceed
	barrier_buffer[id-1] = 1;
}

/* Barrier synchronization called by the master thread */
void master_sync() {
	int i, sum;

	do {
		// Check ready state of all worker threads.
		for(i=0, sum=1; i<NUM_THREADS - 1; i++) {
			sum += barrier_buffer[i];
		}
	} while(sum < NUM_THREADS);

	// Clear ready states
	for(i=0; i<NUM_THREADS - 1; i++) {
		barrier_buffer[i] = 0;
	}
}

void signal_do_instruction() {
	job_cycle = !job_cycle;
}

void do_work(int id) {
	instruction* inst = &global_instruction;

	inst->work(inst->dist, id, inst->output, inst->f, inst->p, inst->args, inst->cmp);
}

void* worker(void* args) {
	int 		id;
	char my_cycle;
       

	//id = *(int*)args;
	id = (int)args;
	my_cycle = 0;

	// Need to sync after initialization
	worker_sync(id);

	for(;;) {
		// Busy wait until current job cycle has changed
		while(my_cycle == job_cycle);
		
		if(kill_threads == 1)
			break;

		my_cycle = job_cycle;
		do_work(id);

		// Sync again before proceeding to wait for next instruction
		worker_sync(id);
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
	
	barrier_buffer[id - 1] = 0;
}

void init_threadpool() {
	for(int i = 0; i < NUM_THREADS - 1; i++) {
		init_thread(i + 1, thrds + i);
	}

	job_cycle = 0;

	// Wait until all workers have been initialized
	master_sync();
}

void kill_threadpool() {
	kill_threads = 1;

	signal_do_instruction();
	for(int i = 0; i < NUM_THREADS - 1; i++)
		pthread_join(thrds[i], NULL);
}

void execute_in_parallel(void (*work)(distribution dist, int id, par_array* out, void* f, void* p, void* args, void* cmp), distribution dist, par_array* out, void* f, void* p, void* args, void* cmp) {

	global_instruction.work = work;
	global_instruction.f 	= f;
	global_instruction.p	= p;
	global_instruction.args = args;
	global_instruction.dist = dist;
	global_instruction.output = out;
	global_instruction.cmp = cmp;
	
	signal_do_instruction();
	do_work(0);
	master_sync();
}
