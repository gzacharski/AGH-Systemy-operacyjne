CC=gcc
CFLAGS=-Wall -g

all: server.o client.o
	$(CC) $(CFLAGS) server.o -o server
	$(CC) $(CFLAGS) client.o -o client

server.o: server.c messaging.h
	$(CC) $(CFLAGS) -c server.c

client.o: client.c messaging.h
	$(CC) $(CFLAGS) -c client.c

clean:
	rm *.o server client