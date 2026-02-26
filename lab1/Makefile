CC=g++
INC=-I./HashTable

all: main

main: src/main.cpp
	$(CC) src/main.cpp -o ./main  -g $(INC)

test: ./main
	perf record -F 99 -g -O2 ./main
	perf script > out.perf
	./FlameGraph/stackcollapse-perf.pl out.perf | ./FlameGraph/flamegraph.pl > flamegraph.svg
