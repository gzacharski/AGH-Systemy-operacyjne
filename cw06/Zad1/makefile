CC=gcc
CFLAGS=-Wall -g

all: golibroda.o klienci.o 
	$(CC) $(CFLAGS) golibroda.o -o golibroda
	$(CC) $(CFLAGS) klienci.o -o klienci

golibroda.o: golibroda.c
	$(CC) $(CFLAGS) -c golibroda.c

klienci.o: klienci.c
	$(CC) $(CFLAGS) -c klienci.c

shared_memory.o: shared_memory.c
	$(CC) $(CFLAGS) -c shared_memory.c

clean:
	rm *.o golibroda klienci