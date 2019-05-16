#include "parallel.h"

#include "threading.h"

void vb_init_par_env() {
	init_threadpool();
}
void vb_destroy_par_env() {
	kill_threadpool();
}


