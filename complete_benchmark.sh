#!/bin/bash

if [ $# -lt 1 ] 
then
	echo "Usage: $0 <number of cores to run on>"
	exit
fi

trap "echo 'Failed' >> benchmark.txt; exit 1" ERR 

> benchmark.txt

for i in `seq 1 $1`;
do
	echo "#### BENCH ON $i CORES ####" >> benchmark.txt
	cd lib/runtime 
	eval make clean && make libvb_par.a -e T=$i 
	cd ../..
	eval make clean && make test_bench	
	echo "Sleeping for 5 seconds before running the test..."
	sleep 5 
	echo "OK, running test on $i core(s)"
	eval ./bin/parallel_benchmark >> benchmark.txt 2> >(sed $'s,.*,\e[31m&\e[m,'>&2)
done

