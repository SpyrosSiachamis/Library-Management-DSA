CC = gcc
CFLAGS = -g

all: main

main: main.o
	$(CC) $(CFLAGS) main.o -o main

main.o: main.c Library.h
	$(CC) $(CFLAGS) -c main.c

run1: main test1
	./main test1

run2: main test2
	./main test2

clean:
	rm -f *.o main *.out