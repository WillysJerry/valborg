#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "../threading.h"

typedef struct _instruction {
	void (*work)(distribution dist, int id, par_array* out, void* f, void* p, void* args, void* cmp);	// Work to be executed on each thread
	void* f;				// Optional function
	void* p;				// Optional predicate
	void* args;
	void* cmp;
	distribution dist;
	par_array* output;
} instruction;

void init_threadpool();
void kill_threadpool();

void execute_in_parallel(void (*work)(distribution dist, int id, par_array* out, void* f, void* p, void* args, void* cmp), distribution dist, par_array* out, void* f, void* p, void* args, void* cmp);
#endif // THREADPOOL_H
