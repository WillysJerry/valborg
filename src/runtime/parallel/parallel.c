#include "parallel.h"

#include "threading.h"

void init_par_env() {
	init_threadpool();
}
void destroy_par_env() {
	kill_threadpool();
}


