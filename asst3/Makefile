all: client server

client: DUMB.o DUMBclient.c
	gcc -g DUMBclient.c DUMB.o -o DUMBclient

server: DUMB.o DUMBserver.c
	gcc -g DUMBserver.c DUMB.o -o DUMBserver -lpthread 

DUMB.o: DUMB.c
	gcc -g -c DUMB.c -lpthread

clean:
	rm DUMBserver DUMBclient DUMB.o
