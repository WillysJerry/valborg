CC=gcc
CFLAGS=-Wall -O3 -g
INCLUDE=-I ./lib/runtime/include
LIB=-L./lib/runtime -lvb_par -lpthread -lm 

all: libs tests examples

libs:
	cd lib/runtime && make all && cd ../..

tests: test_asn test_abs test_bench test_communication test_concat test_map test_reduce test_scan test_map_bench

examples: example_vectorscalarmul example_abs example_dot example_fir-filter

test_map: tests/map.c 
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/map $(INCLUDE) $(LIB)

test_reduce: tests/reduce.c 
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/reduce $(INCLUDE) $(LIB)
test_scan: tests/scan.c 
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/scan $(INCLUDE) $(LIB)

test_concat: tests/concat.c 
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/concat $(INCLUDE) $(LIB)

test_communication: tests/communication.c 
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/communication $(INCLUDE) $(LIB)

test_asn: tests/asn.c
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/asn $(INCLUDE) $(LIB)

test_abs: tests/abs.c
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/abs $(INCLUDE) $(LIB)

test_bench: tests/bench.c
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/parallel_benchmark $(INCLUDE) $(LIB)

test_map_bench: tests/map2_size_bench.c
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/size_benchmark $(INCLUDE) $(LIB)

example_vectorscalarmul: examples/vectorscalarmul.c
	mkdir -p bin
	mkdir -p bin/examples
	$(CC) $(CFLAGS) $^ -o bin/examples/vec_sca_mul $(INCLUDE) $(LIB)

example_abs: examples/abs.c
	mkdir -p bin
	mkdir -p bin/examples
	$(CC) $(CFLAGS) $^ -o bin/examples/abs $(INCLUDE) $(LIB)

example_dot: examples/dot.c
	mkdir -p bin
	mkdir -p bin/examples
	$(CC) $(CFLAGS) $^ -o bin/examples/dot $(INCLUDE) $(LIB)

example_fir-filter: examples/fir-filter.c
	mkdir -p bin
	mkdir -p bin/examples
	$(CC) $(CFLAGS) $^ -o bin/examples/fir-filter $(INCLUDE) $(LIB)
clean:
	rm -rf bin/* obj/*

.PHONY: all tests clean
