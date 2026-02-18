CC=g++

LOG= -D LOG_ON -D LOG_E
INC=-I./HashTable
DEBUG=-g

SRC=main.cpp
OBJ= $(SRC:%.cpp=obj/%.o)

all: $(OBJ)
	$(CC) $^ -o main.exe $(DEBUG)

obj/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(DEBUG) $(INC)
