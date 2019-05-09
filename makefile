CC=gcc
CFLAGS=-Wall -O3 -g
LIB=-lpthread -lm

GLOB_OBJ=obj/runtime.o
SEQ_OBJ=obj/seq_sequential.o
PAR_OBJ=obj/par_fast_barrier.o obj/par_threadpool.o obj/par_threading.o obj/par_parallel.o obj/par_reduce.o obj/par_get.o obj/par_concat.o obj/par_map.o obj/par_count.o obj/par_select.o obj/par_send.o obj/par_scan.o obj/par_asn.o


# Probably a good idea to make the "runtime" into a library or something instead of directly linking the object files (mostly because it's annoying and looks ugly, and also because it makes the most sense?)

tests: test_reduce test_communication test_concat test_map test_scan test_asn

# Prefix all sequential runtime object files with seq_
obj/seq_%.o: src/runtime/sequential/%.c
	mkdir -p obj
	$(CC) $(CFLAGS) -c $^ -o $@ 

# Prefix all parallel runtime object files with seq_
obj/par_%.o: src/runtime/parallel/%.c
	mkdir -p obj
	$(CC) $(CFLAGS) -c $^ -o $@ 

# Shared interface between both the parallel and sequential runtime
obj/runtime.o: src/runtime/runtime.c
	mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@ 

test_a: $(GLOB_OBJ) $(SEQ_OBJ) tests/a.c
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/a $(LIB)

test_map: $(GLOB_OBJ) $(SEQ_OBJ) $(PAR_OBJ) tests/map.c 
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/map $(LIB)

test_reduce: $(GLOB_OBJ) $(SEQ_OBJ) $(PAR_OBJ) tests/reduce.c 
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/reduce $(LIB) 
test_scan: $(GLOB_OBJ) $(SEQ_OBJ) $(PAR_OBJ) tests/scan.c 
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/scan $(LIB) 

test_concat: $(GLOB_OBJ) $(SEQ_OBJ) $(PAR_OBJ) tests/concat.c 
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/concat $(LIB) 

test_communication: $(GLOB_OBJ) $(SEQ_OBJ) $(PAR_OBJ) tests/communication.c 
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/communication $(LIB) 

test_asn: $(GLOB_OBJ) $(SEQ_OBJ) $(PAR_OBJ) tests/asn.c
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/asn $(LIB) 

clean:
	rm -rf bin/* obj/*

.PHONY: all tests clean
