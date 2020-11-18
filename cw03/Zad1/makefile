CC=gcc
CFLAGS=-Wall -g

all: main.o
	$(CC) $(FLAGS) main.o -o main

main.o: main.c
	$(CC) $(FLAGS) -c main.c

clean:
	rm *.o main