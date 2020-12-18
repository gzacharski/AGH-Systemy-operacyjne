CC=gcc 
CFLAGS=-Wall -g

all: server client

server: server.c messaging.h
	$(CC) $(CFLAGS) -o server server.c -lrt

client: client.c
	$(CC) $(CFLAGS) -o client client.c -lrt

clean:
	rm *.o server client