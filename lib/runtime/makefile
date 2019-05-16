ifndef T
# Default number of threads to number of processors if OS is supported, otherwise run on 1 core
NPROCS=1
OS=$(shell uname -s)

ifeq ($(OS),Linux)	# Linux
NPROCS=$(shell grep -c ^processor /proc/cpuinfo)
endif

ifeq ($(OS),Darwin)	# Mac OS X
NPROCS=$(shell system_profiler | awk '/Number Of CPUs/{print $4}{next;}')
endif

T=$(NPROCS)
endif

CC=gcc
CFLAGS=-Wall -O3 -g -DNUM_THREADS=$(T)
INCLUDE=-I./include
LIB=-lpthread -lm

GLOB_OBJ=obj/runtime.o
SEQ_OBJ=obj/seq_sequential.o

PRIM_OBJ=obj/par_reduce.o obj/par_get.o obj/par_concat.o obj/par_map.o obj/par_count.o obj/par_select.o obj/par_send.o obj/par_scan.o obj/par_asn.o obj/par_replicate.o

PTHRD_OBJ=obj/par_tp_pthreads.o obj/par_threading.o obj/par_parallel.o $(PRIM_OBJ)
LCKLSS_OBJ=obj/par_tp_lockless.o obj/par_threading.o obj/par_parallel.o $(PRIM_OBJ)

#PAR_OBJ=$(PTHRD_OBJ)
PAR_OBJ=$(LCKLSS_OBJ)

all: libvb_par.a libvb_seq.a

libvb_par.a: $(GLOB_OBJ) $(PAR_OBJ)
	ar rcs $@ $^

libvb_seq.a: $(GLOB_OBJ) $(SEQ_OBJ)
	ar rcs $@ $^

# Prefix all sequential runtime object files with seq_
obj/seq_%.o: src/sequential/%.c
	mkdir -p obj
	$(CC) $(CFLAGS) -c $^ -o $@ $(INCLUDE)

# Prefix all parallel runtime object files with par_
obj/par_%.o: src/parallel/%.c
	mkdir -p obj
	$(CC) $(CFLAGS) -c $^ -o $@ $(INCLUDE)
obj/par_%.o: src/parallel/threadpool/%.c
	mkdir -p obj
	$(CC) $(CFLAGS) -c $^ -o $@ $(INCLUDE)
obj/par_%.o: src/parallel/primitives/%.c
	mkdir -p obj
	$(CC) $(CFLAGS) -c $^ -o $@ $(INCLUDE)


# Shared interface between both the parallel and sequential runtime
obj/runtime.o: src/runtime.c
	mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE)
clean:
	rm -rf *.a obj/* 

.PHONY: all tests clean
