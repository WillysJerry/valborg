CC=gcc
CFLAGS=-Wall -O3 -g
LIB=-lpthread

# Probably a good idea to make the "runtime" into a library or something instead of directly linking the object files (mostly because it's annoying and looks ugly, and also because it makes the most sense?)
tests: test_a test_map

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

test_a: obj/runtime.o obj/seq_sequential.o tests/a.c
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/a
test_map: obj/runtime.o obj/seq_sequential.o obj/par_threading.o obj/par_parallel.o tests/map.c
	mkdir -p bin
	$(CC) $(CFLAGS) $(LIB) $^ -o bin/map

clean:
	rm -rf bin/* obj/*

.PHONY: all tests clean
