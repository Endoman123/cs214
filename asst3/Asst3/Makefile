all: bankingServer bankingClient

bankingServer: bankingServer.c
	gcc -g -Wall -Werror -fsanitize=address -o bankingServer bankingServer.c -pthread -lrt

bankingClient: bankingClient.c
	gcc -g -Wall -Werror -fsanitize=address -o bankingClient bankingClient.c -pthread -lrt

clean:
	rm bankingClient
	rm bankingServer
