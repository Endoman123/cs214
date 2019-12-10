all: client server

client: DUMBclient.c
	gcc -g DUMBclient.c -o DUMBclient

server: DUMBserver.c
	gcc -g DUMBserver.c -o DUMBserver -lpthread 

clean:
	rm DUMBserver DUMBclient
