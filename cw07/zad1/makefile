CC = gcc
CFLAGS = -g -O2 -std=gnu99 -Wall -lpthread

all: main

main: main.c
	$(CC) $< -o $@ $(CFLAGS)

test: 
	./main config

clean:
	rm *.o main
