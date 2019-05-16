CC=gcc
CFLAGS=-Wall -O3 -g
INCLUDE=-I ./lib/runtime/include
LIB=-lpthread -lm -L./lib/runtime -lvb_par

all: libs tests

libs:
	cd lib/runtime && make all && cd ../..

tests: test_asn test_abs test_bench test_communication test_concat test_map test_reduce test_scan algorithms

algorithms: algorithm_vectorscalarmul algorithm_abs algorithm_dot

test_map: tests/map.c 
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/map $(LIB) $(INCLUDE)

test_reduce: tests/reduce.c 
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/reduce $(LIB) $(INCLUDE)
test_scan: tests/scan.c 
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/scan $(LIB) $(INCLUDE)

test_concat: tests/concat.c 
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/concat $(LIB) $(INCLUDE)

test_communication: tests/communication.c 
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/communication $(LIB) $(INCLUDE)

test_asn: tests/asn.c
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/asn $(LIB) $(INCLUDE)

test_abs: tests/abs.c
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/abs $(LIB) $(INCLUDE)

test_bench: tests/bench.c
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o bin/parallel_benchmark $(LIB) $(INCLUDE)

algorithm_vectorscalarmul: algorithms/vectorscalarmul.c
	mkdir -p bin
	mkdir -p bin/algorithms
	$(CC) $(CFLAGS) $^ -o bin/algorithms/vec_sca_mul $(LIB) $(INCLUDE)

algorithm_abs: algorithms/abs.c
	mkdir -p bin
	mkdir -p bin/algorithms
	$(CC) $(CFLAGS) $^ -o bin/algorithms/abs $(LIB) $(INCLUDE)

algorithm_dot: algorithms/dot.c
	mkdir -p bin
	mkdir -p bin/algorithms
	$(CC) $(CFLAGS) $^ -o bin/algorithms/dot $(LIB) $(INCLUDE)
clean:
	rm -rf bin/* obj/*

.PHONY: all tests clean
