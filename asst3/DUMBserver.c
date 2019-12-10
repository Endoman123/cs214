#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ctype.h>
#include <signal.h>
#include "DUMB.h"

const char* IP_ADDR = "127.0.0.1";

const int MAX_QUEUE = 20;

void* handleClient(void*);
int receiveClientMessage(int, char**);

int main(int argc, char **argv) {
    //User input for the server should be a port number.
    if (argc != 2) {
        printf("Error: Incorrect number of arguments.\n");
    }
    
    char* port = argv[1]; //Parse the port from the user input
    
    //Stop SIGPIPES from stopping the server 
    //SIGPIPE occurs when a program tries to send or recv to or from a socket that is no longer available/connected
    //The default action for a SIGPIPE is to kill the program
    //We have to override this behavior so a SIGPIPE does nothing whatsoever.
    sigaction(SIGPIPE, &(struct sigaction){SIG_IGN}, NULL);

    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error: Socket could not be created.\n");
    }
    
    struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM 
    };

    struct addrinfo* info;
    int addr_error;
    if ((addr_error = getaddrinfo(NULL, port, &hints, &info)) != 0) {
        printf("Error: Could not obtain address information.\n");
        return -1;
    }

    int error;
    if ((error = bind(sock, info -> ai_addr, info -> ai_addrlen)) < 0) {
        printf("Error: Could not bind socket to port.\n");
        return -1;
    }
      
    freeaddrinfo(info);
    
    //Set the socket to listen for connections.
    //We also have to specify the maximum number of connections that can be queued for listening
    listen(sock, MAX_QUEUE);

    //All of our interactions with the client have to be concurrent with other clients
    //For this, lets have threads do the concurrency 
    
    struct sockaddr_storage conn_addr;
    int addressSize = sizeof(conn_addr);
    int conn;
    pthread_t thread;
    printf("Looking for connections from clients...\n");
    while ((conn = accept(sock, (struct sockaddr*)&conn_addr, &addressSize))) {
        int* client = malloc(sizeof(int));
        *client = conn;
        
        pthread_create(&thread, NULL, handleClient, (void*)client);
        pthread_detach(thread); 
    }
}

void* handleClient(void* args) {
    //Get the socket from the args
    int sock = *((int*) args);

    while (1) {
        char* clientMessage;
        int strLen = receiveMessage(sock, &clientMessage);
  
        if (clientMessage != NULL && clientMessage != "") {
            printf("The client has sent a message!\n");
            printf("Client %d sent the message \"%s\"\n", sock, clientMessage);
        }

        char* serverResponse;
        serverResponse = "Message received";
        send(sock, serverResponse, strlen(serverResponse) + 1, 0);
    }
}
