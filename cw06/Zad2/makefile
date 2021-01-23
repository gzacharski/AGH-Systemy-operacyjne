CC=gcc
CFLAGS=-Wall -g

all: golibroda.o klienci.o 
	$(CC) $(CFLAGS) golibroda.o -o golibroda -lpthread -lrt
	$(CC) $(CFLAGS) klienci.o -o klienci -lpthread -lrt

golibroda.o: golibroda.c
	$(CC) $(CFLAGS) -c golibroda.c

klienci.o: klienci.c
	$(CC) $(CFLAGS) -c klienci.c

clean:
	rm *.o golibroda klienci