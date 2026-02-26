.ONESHELL:

CC=g++
INC=-I./src
FLAGS= -g -march=native

all: build test draw

build: ./src/main.cpp
	$(CC) ./src/main.cpp -o main $(INC) $(FLAGS)

test:
	./main

draw:
	source ./graph_venv/bin/activate
	python3 ./draw_graphs.py
	deactivate
