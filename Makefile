CC = gcc
all: main.o main

run1: main test1
	@./main test1

run2: main test2
	@./main test2
main: main.o
	@$(CC) main.o -o main

main.o: main.c Library.h
	@$(CC) -c main.c
clean:
	@rm -f *.o *.out